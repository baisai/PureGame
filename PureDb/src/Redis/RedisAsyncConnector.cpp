/*
 * Copyright (c) 2023-present ChenDong, email <baisaichen@live.com>. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/PureLog.h"
#include "PureCore/OsHelper.h"
#include "PureCore/SleepIdler.h"
#include "PureMsg/MsgArgs.h"
#include "PureDb/DbErrorDesc.h"
#include "PureDb/Redis/RedisAsyncConnector.h"

namespace PureDb {
void RedisAsyncConnector::stop() {
    mRunning = false;
    join(0);
    mAsyncItemPool.free(mReqing);
    mReqing = nullptr;
    for (const auto& iter : mReqWaiting) {
        mReqPool.free(iter.second);
    }
    mReqWaiting.clear();
    while (!mReqQueue.empty()) {
        mAsyncItemPool.free(mReqQueue.pop_front_t<RedisAsyncItem>());
    }
    mSwapReqMutex.lock();
    while (!mSwapReqQueue.empty()) {
        mAsyncItemPool.free(mSwapReqQueue.pop_front_t<RedisAsyncItem>());
    }
    mSwapReqMutex.unlock();
    mSwapRespMutex.lock();
    while (!mSwapRespQueue.empty()) {
        mAsyncItemPool.free(mSwapRespQueue.pop_front_t<RedisAsyncItem>());
    }
    mSwapRespMutex.unlock();
}

int RedisAsyncConnector::init(int64_t reqTimeout) {
    if (reqTimeout <= 0) {
        return ErrorInvalidArg;
    }
    mReqTimeOut = reqTimeout;
    mRunning = true;
    return Success;
}

void RedisAsyncConnector::update() {
    logic_resp();
    logic_req();
    int64_t timeout = PureCore::steady_milli_s() - mReqTimeOut;
    for (auto iter = mReqWaiting.begin(); iter != mReqWaiting.end();) {
        if (iter->second == nullptr || iter->second->mReqTime < timeout) {
            PureWarn("RedisAsyncConnector req {} waiting from {} timeout", iter->second->mReqID, iter->second->mReqTime);
            mReqPool.free(iter->second);
        } else {
            break;
        }
        iter = mReqWaiting.erase(iter);
    }
}

bool RedisAsyncConnector::is_busying() const { return mBusying.load(std::memory_order_relaxed); }

int RedisAsyncConnector::connect(std::function<void(RedisReplyPtr)> cb, const RedisConfig& cfg) {
    if (!cb) {
        return ErrorInvalidArg;
    }
    auto req = mReqPool.get();
    auto item = mAsyncItemPool.get();
    if (req == nullptr || item == nullptr) {
        mReqPool.free(req);
        mAsyncItemPool.free(item);
        return ErrorMemoryNotEnough;
    }
    int err = item->mMsg.pack(cfg);
    if (err != PureMsg::Success) {
        mReqPool.free(req);
        mAsyncItemPool.free(item);
        return ErrorInvalidArg;
    }
    req->mReqID = mReqGen.gen_id();
    req->mReqTime = PureCore::steady_milli_s();
    req->mResp = cb;
    item->mReqID = req->mReqID;
    item->mType = ERedisAsyncConnect;
    if (!mReqWaiting.insert(std::make_pair(req->mReqID, req)).second) {
        mReqPool.free(req);
        mAsyncItemPool.free(item);
        return ErrorDbAsyncReqFailed;
    }
    mReqQueue.push_back(item);
    return Success;
}

int RedisAsyncConnector::add_pipe(PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params) {
    int err = req_cmd(cmd, params.size());
    if (err != Success) {
        return err;
    }
    for (auto& param : params) {
        err = req_param(param);
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int RedisAsyncConnector::commit_pipe(std::function<void(RedisReplyPtr)> cb) { return req_commit(cb, true); }

void RedisAsyncConnector::cancel_pipe() {
    mAsyncItemPool.free(mReqing);
    mReqing = nullptr;
}

int RedisAsyncConnector::command(std::function<void(RedisReplyPtr)> cb, PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params) {
    if (!cb) {
        return ErrorInvalidArg;
    }
    cancel_pipe();
    int err = req_cmd(cmd, params.size());
    if (err != Success) {
        return err;
    }
    for (auto& param : params) {
        err = req_param(param);
        if (err != Success) {
            return err;
        }
    }
    return req_commit(cb, false);
}

int RedisAsyncConnector::req_cmd(PureCore::StringRef cmd, size_t count) {
    if (mReqing == nullptr) {
        mReqing = mAsyncItemPool.get();
        if (mReqing == nullptr) {
            return ErrorMemoryNotEnough;
        }
    }
    int err = PureMsg::pack_args(mReqing->mMsg, cmd, int32_t(count));
    if (err != PureMsg::Success) {
        mAsyncItemPool.free(mReqing);
        mReqing = nullptr;
        return ErrorDbAsyncReqFailed;
    }
    return Success;
}

int RedisAsyncConnector::req_param(PureCore::StringRef param) {
    if (mReqing == nullptr) {
        return ErrorRedisStateInvalid;
    }
    int err = mReqing->mMsg.pack(param);
    if (err != PureMsg::Success) {
        mAsyncItemPool.free(mReqing);
        mReqing = nullptr;
        return ErrorDbAsyncReqFailed;
    }
    return Success;
}

int RedisAsyncConnector::req_params() { return Success; }

int RedisAsyncConnector::req_commit(std::function<void(RedisReplyPtr)> cb, bool isPipe) {
    if (mReqing == nullptr) {
        return ErrorRedisStateInvalid;
    }
    if (!cb) {
        mAsyncItemPool.free(mReqing);
        mReqing = nullptr;
        return ErrorInvalidArg;
    }
    auto req = mReqPool.get();
    if (req == nullptr) {
        mAsyncItemPool.free(mReqing);
        mReqing = nullptr;
        return ErrorMemoryNotEnough;
    }
    req->mReqID = mReqGen.gen_id();
    req->mReqTime = PureCore::steady_milli_s();
    req->mResp = cb;
    mReqing->mReqID = req->mReqID;
    mReqing->mType = isPipe ? ERedisAsyncPipe : ERedisAsyncCommand;
    if (!mReqWaiting.insert(std::make_pair(req->mReqID, req)).second) {
        mReqPool.free(req);
        mAsyncItemPool.free(mReqing);
        mReqing = nullptr;
        return ErrorDbAsyncReqFailed;
    }
    mReqQueue.push_back(mReqing);
    mReqing = nullptr;
    return Success;
}

void RedisAsyncConnector::logic_req() {
    if (mReqQueue.empty()) {
        return;
    }
    mSwapReqMutex.lock();
    mSwapReqQueue.push_back_list(mReqQueue);
    mSwapReqMutex.unlock();
}

void RedisAsyncConnector::logic_resp() {
    PureCore::NodeList nl;
    mSwapRespMutex.lock();
    nl.push_back_list(mSwapRespQueue);
    mSwapRespMutex.unlock();
    while (!nl.empty()) {
        RedisAsyncItem* item = nl.pop_front_t<RedisAsyncItem>();
        if (item == nullptr) {
            continue;
        }
        if (!item->mReply) {
            mAsyncItemPool.free(item);
            PureError("RedisAsyncConnector logic_resp reply nullptr");
            continue;
        }
        auto iter = mReqWaiting.find(item->mReqID);
        if (iter != mReqWaiting.end()) {
            iter->second->mResp(item->mReply);
            mReqPool.free(iter->second);
            mReqWaiting.erase(iter);
        } else {
            PureError("RedisAsyncConnector process_resp not found req {}, maybe this req timeout", item->mReqID);
        }
        mAsyncItemPool.free(item);
    }
}

void RedisAsyncConnector::work_req() {
    PureCore::NodeList nl;
    mSwapReqMutex.lock();
    nl.push_back_list(mSwapReqQueue);
    mSwapReqMutex.unlock();
    while (!nl.empty()) {
        RedisAsyncItem* item = nl.pop_front_t<RedisAsyncItem>();
        if (item == nullptr) {
            continue;
        }
        if (item->mMsg.size() == 0) {
            mRespQueue.push_back(item);
            PureError("RedisAsyncConnector work_req item msg empty");
            continue;
        }
        switch (item->mType) {
            case ERedisAsyncConnect: {
                item->mReply = RedisReply::get();
                do {
                    RedisConfig cfg;
                    int err = item->mMsg.unpack(cfg);
                    if (err != PureMsg::Success) {
                        item->mReply->set_error(get_error_desc(ErrorDbAsyncReqFailed));
                        break;
                    }
                    err = mConnector.connect(cfg);
                    if (err != Success) {
                        item->mReply->set_error(get_error_desc(err));
                        break;
                    }
                } while (false);
                mRespQueue.push_back(item);
            } break;
            case ERedisAsyncCommand:
            case ERedisAsyncPipe: {
                bool isPipe = item->mType == ERedisAsyncPipe;
                int32_t count = 0;
                PureCore::StringRef cmd;
                PureCore::StringRef param;
                int err = Success;
                while (item->mMsg.size() > 0 && err == Success) {
                    size_t pos = item->mMsg.read_pos();
                    err = PureMsg::unpack_args(item->mMsg, cmd, count);
                    if (err != PureMsg::Success) {
                        err = ErrorInvalidArg;
                        break;
                    }
                    mParamsCache.clear();
                    for (int32_t i = 0; i < count; ++i) {
                        err = item->mMsg.unpack(param);
                        if (err != PureMsg::Success) {
                            err = ErrorInvalidArg;
                            break;
                        }
                        mParamsCache.push_back(param);
                    }
                    if (err == Success) {
                        if (isPipe) {
                            err = mConnector.pipe(cmd, mParamsCache);
                            if (err != Success) {
                                break;
                            }
                        } else {
                            err = mConnector.command(cmd, mParamsCache);  // only once command
                            break;
                        }
                    }
                }
                if (isPipe) {
                    item->mReply = RedisReply::get();
                    if (err != Success) {
                        item->mReply->set_error(get_error_desc(err));
                    } else {
                        while (true) {
                            auto sub = mConnector.pop_reply();
                            if (!sub) {
                                break;
                            }
                            item->mReply->append_array(sub);
                        }
                    }
                } else {
                    auto reply = mConnector.pop_reply();
                    if (!reply) {
                        item->mReply = RedisReply::get();
                        if (err != Success) {
                            item->mReply->set_error(get_error_desc(err));
                        }
                    } else {
                        item->mReply = reply;
                    }
                }
                mRespQueue.push_back(item);
            } break;
            default:
                mRespQueue.push_back(item);
                PureError("RedisAsyncConnector work_req async type error {}", int(item->mType));
                break;
        }
    }
}

void RedisAsyncConnector::work_resp() {
    if (mRespQueue.empty()) {
        return;
    }
    mSwapRespMutex.lock();
    mSwapRespQueue.push_back_list(mRespQueue);
    mSwapRespMutex.unlock();
}

void RedisAsyncConnector::work() {
    PureCore::SleepIdler idle;
    idle.set_idle_delay(10 * 1000);
    PureCore::NodeList req;
    while (mRunning.load(std::memory_order_relaxed)) {
        idle.frame_begin();
        work_req();
        mConnector.update();
        work_resp();
        idle.frame_end();
    }
    mConnector.close();
    work_resp();
    mParamsCache.clear();
}
}  // namespace PureDb
