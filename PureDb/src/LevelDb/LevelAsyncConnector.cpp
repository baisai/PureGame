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
#include "PureDb/LevelDb/LevelAsyncConnector.h"

namespace PureDb {
void LevelAsyncConnector::stop() {
    mRunning = false;
    join(0);
    for (const auto& iter : mReqWaiting) {
        mReqPool.free(iter.second);
    }
    mReqWaiting.clear();
    while (!mReqQueue.empty()) {
        mAsyncItemPool.free(mReqQueue.pop_front_t<LevelAsyncItem>());
    }
    mSwapReqMutex.lock();
    while (!mSwapReqQueue.empty()) {
        mAsyncItemPool.free(mSwapReqQueue.pop_front_t<LevelAsyncItem>());
    }
    mSwapReqMutex.unlock();
    mSwapRespMutex.lock();
    while (!mSwapRespQueue.empty()) {
        mAsyncItemPool.free(mSwapRespQueue.pop_front_t<LevelAsyncItem>());
    }
    mSwapRespMutex.unlock();
}

int LevelAsyncConnector::init(int64_t reqTimeout) {
    if (reqTimeout <= 0) {
        return ErrorInvalidArg;
    }
    mReqTimeOut = reqTimeout;
    mRunning = true;
    return Success;
}

void LevelAsyncConnector::update() {
    logic_resp();
    logic_req();
    int64_t timeout = PureCore::steady_milli_s() - mReqTimeOut;
    for (auto iter = mReqWaiting.begin(); iter != mReqWaiting.end();) {
        if (iter->second == nullptr || iter->second->mReqTime < timeout) {
            PureWarn("LevelAsyncConnector req {} waiting from {} timeout", iter->second->mReqID, iter->second->mReqTime);
            mReqPool.free(iter->second);
        } else {
            break;
        }
        iter = mReqWaiting.erase(iter);
    }
}

int LevelAsyncConnector::connect(std::function<void(LevelReplyPtr)> cb, const LevelConfig& cfg) {
    return make_req(ELevelAsyncConnect, cb, [&cfg](PureMsg::MsgDynamicBuffer& buffer) {
        int err = buffer.pack(cfg);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::vset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef data) {
    return make_req(ELevelAsyncVset, cb, [&name, &data](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, data);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}
int LevelAsyncConnector::vdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncVdel, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::vget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncVget, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::vmset(std::function<void(LevelReplyPtr)> cb, PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas) {
    return make_req(ELevelAsyncVmset, cb, [&datas](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, datas);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::vmdel(std::function<void(LevelReplyPtr)> cb, PureCore::ArrayRef<PureCore::StringRef> names) {
    return make_req(ELevelAsyncVmdel, cb, [&names](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, names);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::vmget(std::function<void(LevelReplyPtr)> cb, PureCore::ArrayRef<PureCore::StringRef> names) {
    return make_req(ELevelAsyncVmget, cb, [&names](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, names);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::v_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncVApproximateSize, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::create_v_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef start, PureCore::StringRef end, bool forward) {
    return make_req(ELevelAsyncIterCreateV, cb, [&start, &end, forward](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, start, end, forward);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key, PureCore::StringRef data) {
    return make_req(ELevelAsyncHset, cb, [&name, &key, &data](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, key, data);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key) {
    return make_req(ELevelAsyncHdel, cb, [&name, &key](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, key);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key) {
    return make_req(ELevelAsyncHget, cb, [&name, &key](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, key);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hmset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name,
                               PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas) {
    return make_req(ELevelAsyncHmset, cb, [&name, &datas](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, datas);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hmdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) {
    return make_req(ELevelAsyncHmdel, cb, [&name, &keys](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, keys);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hmget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) {
    return make_req(ELevelAsyncHmget, cb, [&name, &keys](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, keys);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hgetall(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncHgetall, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::hdrop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncHdrop, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::h_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncHApproximateSize, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::create_h_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end,
                                           bool forward) {
    return make_req(ELevelAsyncIterCreateH, cb, [&name, &start, &end, forward](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, start, end, forward);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::sset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key) {
    return make_req(ELevelAsyncSset, cb, [&name, &key](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, key);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::sdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key) {
    return make_req(ELevelAsyncSdel, cb, [&name, &key](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, key);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::sget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key) {
    return make_req(ELevelAsyncSget, cb, [&name, &key](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, key);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::smset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) {
    return make_req(ELevelAsyncSmset, cb, [&name, &keys](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, keys);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::smdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) {
    return make_req(ELevelAsyncSmdel, cb, [&name, &keys](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, keys);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::sgetall(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncSgetall, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::sdrop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncSdrop, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::s_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncSApproximateSize, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::create_s_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end,
                                           bool forward) {
    return make_req(ELevelAsyncIterCreateS, cb, [&name, &start, &end, forward](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, start, end, forward);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::lset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t idx, PureCore::StringRef data) {
    return make_req(ELevelAsyncLset, cb, [&name, idx, &data](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, idx, data);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::llpush(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef data) {
    return make_req(ELevelAsyncLlpush, cb, [&name, &data](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, data);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::llpop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncLlpop, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::lrpush(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef data) {
    return make_req(ELevelAsyncLrpush, cb, [&name, &data](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, data);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::lrpop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncLrpop, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::lget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t idx) {
    return make_req(ELevelAsyncLget, cb, [&name, idx](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, idx);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::lrange(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t start, int64_t end) {
    return make_req(ELevelAsyncLrange, cb, [&name, start, end](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, start, end);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::ldel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t count, PureCore::StringRef data) {
    return make_req(ELevelAsyncLdel, cb, [&name, count, &data](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, count, data);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::ltrim(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t start, int64_t end) {
    return make_req(ELevelAsyncLtrim, cb, [&name, start, end](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, start, end);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::lgetall(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncLgetall, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::ldrop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncLdrop, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::l_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncLApproximateSize, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::create_l_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, bool forward) {
    return make_req(ELevelAsyncIterCreateL, cb, [&name, forward](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name, forward);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::approximate_size(std::function<void(LevelReplyPtr)> cb) {
    return make_req(ELevelAsyncHmset, cb, [](PureMsg::MsgDynamicBuffer& buffer) { return Success; });
}

int LevelAsyncConnector::property(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name) {
    return make_req(ELevelAsyncProperty, cb, [&name](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, name);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::compact() {
    // not need resp
    auto item = mAsyncItemPool.get();
    if (item == nullptr) {
        return ErrorMemoryNotEnough;
    }
    item->mReqID = 0;
    item->mType = ELevelAsyncCompact;
    mReqQueue.push_back(item);
    return Success;
}

// batch < 0, all
int LevelAsyncConnector::backup_to(std::function<void(LevelReplyPtr)> cb, const char* path, int64_t batch, ELevelBackupCompress compress) {
    if (path == nullptr) {
        return ErrorInvalidArg;
    }
    return make_req(ELevelAsyncBackupTo, cb, [path, batch, compress](PureMsg::MsgDynamicBuffer& buffer) {
        PureCore::StringRef p(path, strlen(path) + 1);
        int err = PureMsg::pack_args(buffer, p, batch, int8_t(compress));
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::iterator(std::function<void(LevelReplyPtr)> cb, int64_t id, size_t count, bool forward) {
    if (id <= 0 || count == 0) {
        return ErrorInvalidArg;
    }
    return make_req(ELevelAsyncIterVist, cb, [id, count, forward](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, id, count, forward);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

int LevelAsyncConnector::remove_iterator(std::function<void(LevelReplyPtr)> cb, int64_t id) {
    if (id <= 0) {
        return ErrorInvalidArg;
    }
    return make_req(ELevelAsyncIterClear, cb, [id](PureMsg::MsgDynamicBuffer& buffer) {
        int err = PureMsg::pack_args(buffer, id);
        if (err != PureMsg::Success) {
            return ErrorDbAsyncReqFailed;
        }
        return Success;
    });
}

void LevelAsyncConnector::logic_req() {
    if (mReqQueue.empty()) {
        return;
    }
    mSwapReqMutex.lock();
    mSwapReqQueue.push_back_list(mReqQueue);
    mSwapReqMutex.unlock();
}

void LevelAsyncConnector::logic_resp() {
    PureCore::NodeList nl;
    mSwapRespMutex.lock();
    nl.push_back_list(mSwapRespQueue);
    mSwapRespMutex.unlock();
    while (!nl.empty()) {
        LevelAsyncItem* item = nl.pop_front_t<LevelAsyncItem>();
        if (item == nullptr) {
            continue;
        }
        if (!item->mReply) {
            mAsyncItemPool.free(item);
            PureError("LevelAsyncConnector logic_resp reply nullptr");
            continue;
        }
        if (item->mReqID == 0) {
            mAsyncItemPool.free(item);
            continue;
        }
        auto iter = mReqWaiting.find(item->mReqID);
        if (iter != mReqWaiting.end()) {
            iter->second->mResp(item->mReply);
            mReqPool.free(iter->second);
            mReqWaiting.erase(iter);
        } else {
            PureError("LevelAsyncConnector process_resp not found req {}, maybe this req timeout", item->mReqID);
        }
        mAsyncItemPool.free(item);
    }
}

int LevelAsyncConnector::make_req(ELevelAsyncType reqType, std::function<void(LevelReplyPtr)> cb,
                                  std::function<int(PureMsg::MsgDynamicBuffer& buffer)> packer) {
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
    int err = packer(item->mMsg);
    if (err != Success) {
        mReqPool.free(req);
        mAsyncItemPool.free(item);
        return err;
    }
    req->mReqID = mReqGen.gen_id();
    req->mReqTime = PureCore::steady_milli_s();
    req->mResp = cb;
    item->mReqID = req->mReqID;
    item->mType = reqType;
    if (!mReqWaiting.insert(std::make_pair(req->mReqID, req)).second) {
        mReqPool.free(req);
        mAsyncItemPool.free(item);
        return ErrorDbAsyncReqFailed;
    }
    mReqQueue.push_back(item);
    return Success;
}

void LevelAsyncConnector::work_req() {
    PureCore::NodeList nl;
    mSwapReqMutex.lock();
    nl.push_back_list(mSwapReqQueue);
    mSwapReqMutex.unlock();
    while (!nl.empty()) {
        LevelAsyncItem* item = nl.pop_front_t<LevelAsyncItem>();
        if (item == nullptr) {
            continue;
        }

        mRespQueue.push_back(item);
        if (item->mMsg.size() == 0) {
            PureError("LevelAsyncConnector work_req item msg empty");
            item->mReply = LevelReply::get();
            item->mReply->set_error(ErrorInvalidArg);
            continue;
        }
        int err = Success;
        switch (item->mType) {
            case ELevelAsyncConnect: {
                do {
                    LevelConfig cfg;
                    err = item->mMsg.unpack(cfg);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.connect(cfg);
                } while (false);
            } break;
            case ELevelAsyncVset: {
                do {
                    PureCore::StringRef name, data;
                    err = PureMsg::unpack_args(item->mMsg, name, data);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.vset(name, data);
                } while (false);
            } break;
            case ELevelAsyncVdel: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.vdel(name);
                } while (false);
            } break;
            case ELevelAsyncVget: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.vget(name);
                } while (false);
            } break;
            case ELevelAsyncVmset: {
                do {
                    std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> datas;
                    err = PureMsg::unpack_args(item->mMsg, datas);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.vmset(datas);
                } while (false);
            } break;
            case ELevelAsyncVmdel: {
                do {
                    std::vector<PureCore::StringRef> names;
                    err = PureMsg::unpack_args(item->mMsg, names);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.vmdel(names);
                } while (false);
            } break;
            case ELevelAsyncVmget: {
                do {
                    std::vector<PureCore::StringRef> names;
                    err = PureMsg::unpack_args(item->mMsg, names);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.vmget(names);
                } while (false);
            } break;
            case ELevelAsyncVApproximateSize: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    size_t s = mConnector.v_approximate_size(name);
                    item->mReply = LevelReply::get();
                    item->mReply->set_int(s);
                } while (false);
            } break;
            case ELevelAsyncHset: {
                do {
                    PureCore::StringRef name, key, data;
                    err = PureMsg::unpack_args(item->mMsg, name, key, data);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hset(name, key, data);
                } while (false);
            } break;
            case ELevelAsyncHdel: {
                do {
                    PureCore::StringRef name, key;
                    err = PureMsg::unpack_args(item->mMsg, name, key);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hdel(name, key);
                } while (false);
            } break;
            case ELevelAsyncHget: {
                do {
                    PureCore::StringRef name, key;
                    err = PureMsg::unpack_args(item->mMsg, name, key);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hget(name, key);
                } while (false);
            } break;
            case ELevelAsyncHmset: {
                do {
                    PureCore::StringRef name;
                    std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> datas;
                    err = PureMsg::unpack_args(item->mMsg, name, datas);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hmset(name, datas);
                } while (false);
            } break;
            case ELevelAsyncHmdel: {
                do {
                    PureCore::StringRef name;
                    std::vector<PureCore::StringRef> names;
                    err = PureMsg::unpack_args(item->mMsg, name, names);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hmdel(name, names);
                } while (false);
            } break;
            case ELevelAsyncHmget: {
                do {
                    PureCore::StringRef name;
                    std::vector<PureCore::StringRef> names;
                    err = PureMsg::unpack_args(item->mMsg, name, names);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hmget(name, names);
                } while (false);
            } break;
            case ELevelAsyncHgetall: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hgetall(name);
                } while (false);
            } break;
            case ELevelAsyncHdrop: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.hdrop(name);
                } while (false);
            } break;
            case ELevelAsyncHApproximateSize: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    size_t s = mConnector.h_approximate_size(name);
                    item->mReply = LevelReply::get();
                    item->mReply->set_int(s);
                } while (false);
            } break;
            case ELevelAsyncSset: {
                do {
                    PureCore::StringRef name, key;
                    err = PureMsg::unpack_args(item->mMsg, name, key);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.sset(name, key);
                } while (false);
            } break;
            case ELevelAsyncSdel: {
                do {
                    PureCore::StringRef name, key;
                    err = PureMsg::unpack_args(item->mMsg, name, key);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.sdel(name, key);
                } while (false);
            } break;
            case ELevelAsyncSget: {
                do {
                    PureCore::StringRef name, key;
                    err = PureMsg::unpack_args(item->mMsg, name, key);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    item->mReply = LevelReply::get();
                    item->mReply->set_bool(mConnector.sget(name, key));
                } while (false);
            } break;
            case ELevelAsyncSmset: {
                do {
                    PureCore::StringRef name;
                    std::vector<PureCore::StringRef> keys;
                    err = PureMsg::unpack_args(item->mMsg, name, keys);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.smset(name, keys);
                } while (false);
            } break;
            case ELevelAsyncSmdel: {
                do {
                    PureCore::StringRef name;
                    std::vector<PureCore::StringRef> keys;
                    err = PureMsg::unpack_args(item->mMsg, name, keys);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.smdel(name, keys);
                } while (false);
            } break;
            case ELevelAsyncSgetall: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.sgetall(name);
                } while (false);
            } break;
            case ELevelAsyncSdrop: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.sdrop(name);
                } while (false);
            } break;
            case ELevelAsyncSApproximateSize: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    size_t s = mConnector.s_approximate_size(name);
                    item->mReply = LevelReply::get();
                    item->mReply->set_int(s);
                } while (false);
            } break;
            case ELevelAsyncLset: {
                do {
                    PureCore::StringRef name, data;
                    int64_t idx = 0;
                    err = PureMsg::unpack_args(item->mMsg, name, idx, data);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.lset(name, idx, data);
                } while (false);
            } break;
            case ELevelAsyncLlpush: {
                do {
                    PureCore::StringRef name, data;
                    err = PureMsg::unpack_args(item->mMsg, name, data);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.llpush(name, data);
                } while (false);
            } break;
            case ELevelAsyncLlpop: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.llpop(name);
                } while (false);
            } break;
            case ELevelAsyncLrpush: {
                do {
                    PureCore::StringRef name, data;
                    err = PureMsg::unpack_args(item->mMsg, name, data);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.lrpush(name, data);
                } while (false);
            } break;
            case ELevelAsyncLrpop: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.lrpop(name);
                } while (false);
            } break;
            case ELevelAsyncLget: {
                do {
                    PureCore::StringRef name;
                    int64_t idx = 0;
                    err = PureMsg::unpack_args(item->mMsg, name, idx);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.lget(name, idx);
                } while (false);
            } break;
            case ELevelAsyncLrange: {
                do {
                    PureCore::StringRef name;
                    int64_t start = 0, end = 0;
                    err = PureMsg::unpack_args(item->mMsg, name, start, end);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.lrange(name, start, end);
                } while (false);
            } break;
            case ELevelAsyncLdel: {
                do {
                    PureCore::StringRef name, data;
                    int64_t count = 0;
                    err = PureMsg::unpack_args(item->mMsg, name, count, data);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.ldel(name, count, data);
                } while (false);
            } break;
            case ELevelAsyncLtrim: {
                do {
                    PureCore::StringRef name;
                    int64_t start = 0, end = 0;
                    err = PureMsg::unpack_args(item->mMsg, name, start, end);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.ltrim(name, start, end);
                } while (false);
            } break;
            case ELevelAsyncLgetall: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.lgetall(name);
                } while (false);
            } break;
            case ELevelAsyncLdrop: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.ldrop(name);
                } while (false);
            } break;
            case ELevelAsyncLApproximateSize: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    size_t s = mConnector.l_approximate_size(name);
                    item->mReply = LevelReply::get();
                    item->mReply->set_int(s);
                } while (false);
            } break;
            case ElevelAsyncApproximateSize: {
                size_t s = mConnector.approximate_size();
                item->mReply = LevelReply::get();
                item->mReply->set_int(s);
            } break;
            case ELevelAsyncProperty: {
                do {
                    PureCore::StringRef name;
                    err = PureMsg::unpack_args(item->mMsg, name);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.property(name);
                } while (false);
            } break;
            case ELevelAsyncCompact: {
                mConnector.compact();
                item->mReply = LevelReply::get();
            } break;
            case ELevelAsyncBackupTo: {
                do {
                    PureCore::StringRef path;
                    int64_t batch = 0;
                    int8_t compress = ELevelCompressLz4;
                    err = PureMsg::unpack_args(item->mMsg, path, batch, compress);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.backup_to(path.data(), batch, ELevelBackupCompress(compress));
                } while (false);
            } break;
            case ELevelAsyncIterCreateV: {
                do {
                    PureCore::StringRef start, end;
                    bool forward = false;
                    err = PureMsg::unpack_args(item->mMsg, start, end, forward);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    int64_t id = mConnector.create_v_iterator(start, end, forward);
                    if (id != 0) {
                        item->mReply = LevelReply::get();
                        item->mReply->set_int(id);
                    }
                } while (false);
            } break;
            case ELevelAsyncIterCreateH: {
                do {
                    PureCore::StringRef name, start, end;
                    bool forward = false;
                    err = PureMsg::unpack_args(item->mMsg, name, start, end, forward);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    int64_t id = mConnector.create_h_iterator(name, start, end, forward);
                    if (id != 0) {
                        item->mReply = LevelReply::get();
                        item->mReply->set_int(id);
                    }
                } while (false);
            } break;
            case ELevelAsyncIterCreateS: {
                do {
                    PureCore::StringRef name, start, end;
                    bool forward = false;
                    err = PureMsg::unpack_args(item->mMsg, name, start, end, forward);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    int64_t id = mConnector.create_s_iterator(name, start, end, forward);
                    if (id != 0) {
                        item->mReply = LevelReply::get();
                        item->mReply->set_int(id);
                    }
                } while (false);
            } break;
            case ELevelAsyncIterCreateL: {
                do {
                    PureCore::StringRef name;
                    bool forward = false;
                    err = PureMsg::unpack_args(item->mMsg, name, forward);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    int64_t id = mConnector.create_l_iterator(name, forward);
                    if (id != 0) {
                        item->mReply = LevelReply::get();
                        item->mReply->set_int(id);
                    }
                } while (false);
            } break;
            case ELevelAsyncIterVist: {
                do {
                    int64_t id = 0;
                    size_t count = 0;
                    bool forward = false;
                    err = PureMsg::unpack_args(item->mMsg, id, count, forward);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    err = mConnector.iterator(id, count, forward);
                } while (false);
            } break;
            case ELevelAsyncIterClear: {
                do {
                    int64_t id = 0;
                    err = PureMsg::unpack_args(item->mMsg, id);
                    if (err != PureMsg::Success) {
                        err = ErrorDbAsyncReqFailed;
                        break;
                    }
                    bool r = mConnector.remove_iterator(id);
                    item->mReply = LevelReply::get();
                    item->mReply->set_bool(r);
                } while (false);
            } break;
            default:
                PureError("LevelAsyncConnector work_req async type error {}", int(item->mType));
                err = ErrorInvalidArg;
                break;
        }  // switch
        if (item->mReply) {
            continue;
        }
        item->mReply = mConnector.pop_reply();
        if (!item->mReply) {
            item->mReply = LevelReply::get();
            if (err != Success) {
                item->mReply->set_error(err);
            } else {
                item->mReply->set_error(ErrorDbReplyNotFound);
            }
        }
    }  // while
}

void LevelAsyncConnector::work_resp() {
    if (mRespQueue.empty()) {
        return;
    }
    mSwapRespMutex.lock();
    mSwapRespQueue.push_back_list(mRespQueue);
    mSwapRespMutex.unlock();
}

void LevelAsyncConnector::work() {
    PureCore::NodeList req;
    PureCore::SleepIdler idle;
    idle.set_idle_delay(10 * 1000);
    while (mRunning.load(std::memory_order_relaxed)) {
        idle.frame_begin();
        work_req();
        mConnector.update();
        work_resp();
        idle.frame_end();
    }
    mConnector.close();
    work_resp();
}
}  // namespace PureDb
