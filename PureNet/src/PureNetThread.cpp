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

#include "PureCore/OsHelper.h"
#include "PureCore/SleepIdler.h"
#include "PureMsg/MsgArgs.h"
#include "PureNet/NetErrorDesc.h"
#include "PureNet/Link.h"
#include "PureNet/PureNetThread.h"

namespace PureNet {
void PureNetThread::set_config(const NetConfig& cfg) { mReacter.set_config(cfg); }

int PureNetThread::start(int64_t reqTimeout) {
    if (reqTimeout <= 0) {
        return ErrorInvalidArg;
    }
    if (is_running()) {
        return ErrorStateError;
    }
    mReqTimeOut = reqTimeout;
    mReacterRunning = true;
    return run();
}

void PureNetThread::stop() {
    if (!is_running()) {
        return;
    }
    mReacterRunning = false;
    join(0);
    mEventLinkOpen.clear();
    mEventLinkStart.clear();
    mEventLinkMsg.clear();
    mEventLinkEnd.clear();
    mEventLinkClose.clear();
    mReqTimeOut = 0;
    for (const auto& iter : mReqWaiting) {
        mReqPool.free(iter.second);
    }
    mReqWaiting.clear();
    while (!mReqQueue.empty()) {
        mAsyncReqPool.free(mReqQueue.pop_front_t<AsyncItem>());
    }
    mSwapReqMutex.lock();
    while (!mSwapReqQueue.empty()) {
        mAsyncReqPool.free(mSwapReqQueue.pop_front_t<AsyncItem>());
    }
    mSwapReqMutex.unlock();
    mSwapRespMutex.lock();
    while (!mSwapRespQueue.empty()) {
        mAsyncRespPool.free(mSwapRespQueue.pop_front_t<AsyncItem>());
    }
    mSwapRespMutex.unlock();
}

int64_t PureNetThread::get_req_timeout() const { return mReqTimeOut; }

void PureNetThread::update() {
    logic_resp();
    logic_req();
    int64_t timeout = PureCore::steady_milli_s() - mReqTimeOut;
    for (auto iter = mReqWaiting.begin(); iter != mReqWaiting.end();) {
        if (iter->second == nullptr || iter->second->mReqTime < timeout) {
            PureWarn("PureNetThread req {} waiting from {} timeout", iter->second->mReqID, iter->second->mReqTime);
            mReqPool.free(iter->second);
        } else {
            break;
        }
        iter = mReqWaiting.erase(iter);
    }
}

void PureNetThread::get_host_ip(const char* host, std::function<GetHostIpCallback> cb) {
    auto req = mAsyncReqPool.get();
    NetMsgPtr msg = NetMsg::get();
    auto waitReq = mReqPool.get();
    int err = 0;
    do {
        if (req == nullptr || !msg || waitReq == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        err = PureMsg::pack_args(*msg, host);
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        req->mReqID = mReqGen.gen_id();
        req->mType = EAsyncGetHostIp;
        req->mMsg = msg;
        waitReq->mReqID = req->mReqID;
        waitReq->mReqTime = PureCore::steady_milli_s();
        waitReq->mResp = [=](NetMsgPtr resp) {
            if (!resp) {
                cb(ErrorNullPointer, "");
                return;
            }
            int respErr = 0;
            std::string ip;
            int err = PureMsg::unpack_args(*resp, respErr, ip);
            if (err != PureMsg::Success) {
                respErr = err;
            }
            cb(respErr, ip.c_str());
        };
        if (!mReqWaiting.insert(std::make_pair(waitReq->mReqID, waitReq)).second) {
            err = ErrorAddNetReqFailed;
            break;
        }
        mReqQueue.push_back(req);
    } while (false);

    if (err != Success) {
        cb(err, "");
        mAsyncReqPool.free(req);
        mReqPool.free(waitReq);
    }
}

void PureNetThread::close_link(LinkID linkID, int reason) {
    auto req = mAsyncReqPool.get();
    NetMsgPtr msg = NetMsg::get();
    int err = 0;
    do {
        if (req == nullptr || !msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        msg->set_link_id(linkID);
        err = PureMsg::pack_args(*msg, reason);
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        req->mType = EAsyncCloseLink;
        req->mMsg = msg;
        mReqQueue.push_back(req);
    } while (false);

    if (err != Success) {
        mAsyncReqPool.free(req);
    }
}

int PureNetThread::send_msg(NetMsgPtr msg) {
    auto req = mAsyncReqPool.get();
    int err = 0;
    do {
        if (req == nullptr || !msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        msg->set_send_flag(ESendSingle);
        req->mType = EAsyncSendMsg;
        req->mMsg = msg;
        mReqQueue.push_back(req);
    } while (false);

    if (err != Success) {
        mAsyncReqPool.free(req);
    }
    return err;
}

int PureNetThread::broadcast_msg(const BroadcastDest& dest, NetMsgPtr msg) {
    auto req = mAsyncReqPool.get();
    NetMsgPtr broadMsg = NetMsg::get();
    int err = 0;
    do {
        if (req == nullptr || !msg || !broadMsg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        msg->set_send_flag(ESendMulti);
        err = PureMsg::pack(*broadMsg, dest);
        if (err != PureMsg::Success) {
            break;
        }
        err = broadMsg->write(msg->data());
        if (err != PureCore::Success) {
            break;
        }
        req->mType = EAsyncSendMsg;
        req->mMsg = broadMsg;
        mReqQueue.push_back(req);
    } while (false);

    if (err != Success) {
        mAsyncReqPool.free(req);
    }
    return err;
}

void PureNetThread::work() {
    int err = mReacter.init();
    if (err != Success) {
        PureError("PureNetThread reacter init failed {}", get_error_desc(err));
        return;
    }
    mReacter.mEventLinkOpen.bind(this, &PureNetThread::on_link_open);
    mReacter.mEventLinkStart.bind(this, &PureNetThread::on_link_start);
    mReacter.mEventLinkMsg.bind(this, &PureNetThread::on_link_msg);
    mReacter.mEventLinkEnd.bind(this, &PureNetThread::on_link_end);
    mReacter.mEventLinkClose.bind(this, &PureNetThread::on_link_close);

    PureCore::SleepIdler idle;
    idle.set_idle_delay(10 * 1000);
    while (mReacterRunning.load(std::memory_order_relaxed)) {
        idle.frame_begin();
        work_req();
        int64_t delta = idle.frame_check(1);
        if (delta > 0) {
            mReacter.update(delta);
        }
        work_resp();
        idle.frame_end();
    }
    mReacter.release();
    work_resp();
}

void PureNetThread::logic_req() {
    if (mReqQueue.empty()) {
        return;
    }
    mSwapReqMutex.lock();
    mSwapReqQueue.push_back_list(mReqQueue);
    mSwapReqMutex.unlock();
}

void PureNetThread::logic_resp() {
    PureCore::NodeList nl;
    mSwapRespMutex.lock();
    nl.push_back_list(mSwapRespQueue);
    mSwapRespMutex.unlock();
    while (!nl.empty()) {
        AsyncItem* item = nl.pop_front_t<AsyncItem>();
        if (item == nullptr) {
            continue;
        }
        if (!item->mMsg) {
            mAsyncReqPool.free(item);
            PureError("PureNetThread logic_resp item msg nullptr");
            continue;
        }
        if (item->mReqID == 0) {
            GroupID groupID = item->mMsg->get_group_id();
            LinkID linkID = item->mMsg->get_link_id();
            switch (item->mType) {
                case PureNet::EAsyncLinkOpen: {
                    char ip[INET6_ADDRSTRLEN]{};
                    int port = 0;
                    int err = PureMsg::unpack_args(*item->mMsg, ip, port);
                    if (err == PureMsg::Success) {
                        mEventLinkOpen.notify(groupID, linkID, ip, port);
                    } else {
                        PureError("PureNetThread logic_resp Link Open unpack failed");
                    }
                    break;
                }
                case PureNet::EAsyncLinkStart:
                    mEventLinkStart.notify(groupID, linkID);
                    break;
                case PureNet::EAsyncLinkMsg:
                    mEventLinkMsg.notify(groupID, linkID, item->mMsg);
                    break;
                case PureNet::EAsyncLinkEnd: {
                    int reason = 0;
                    int err = PureMsg::unpack_args(*item->mMsg, reason);
                    if (err == PureMsg::Success) {
                        mEventLinkEnd.notify(groupID, linkID, reason);
                    } else {
                        PureError("PureNetThread logic_resp Link End unpack failed");
                    }
                    break;
                }
                case PureNet::EAsyncLinkClose: {
                    int reason = 0;
                    int err = PureMsg::unpack_args(*item->mMsg, reason);
                    if (err == PureMsg::Success) {
                        mEventLinkClose.notify(groupID, linkID, reason);
                    } else {
                        PureError("PureNetThread logic_resp Link Close unpack failed");
                    }
                    break;
                }
                default:
                    PureError("PureNetThread logic_resp async type error {}", int(item->mType));
                    break;
            }
        } else {
            auto iter = mReqWaiting.find(item->mReqID);
            if (iter != mReqWaiting.end()) {
                iter->second->mResp(item->mMsg);
                mReqPool.free(iter->second);
                mReqWaiting.erase(iter);
            } else {
                PureError("PureNetThread logic_resp not found req {}, maybe this req timeout", item->mReqID);
            }
        }
        mAsyncReqPool.free(item);
    }
}

void PureNetThread::work_req() {
    PureCore::NodeList nl;
    mSwapReqMutex.lock();
    nl.push_back_list(mSwapReqQueue);
    mSwapReqMutex.unlock();
    while (!nl.empty()) {
        AsyncItem* item = nl.pop_front_t<AsyncItem>();
        if (item == nullptr) {
            continue;
        }
        if (!item->mMsg) {
            mAsyncRespPool.free(item);
            PureError("PureNetThread work_req item msg nullptr");
            continue;
        }
        switch (item->mType) {
            case PureNet::EAsyncListen:
                on_net_listen(item);
                break;
            case PureNet::EAsyncStopListen:
                on_net_stop_listen(item);
                break;
            case PureNet::EAsyncConnect:
                on_net_connect(item);
                break;
            case PureNet::EAsyncGetHostIp:
                on_net_get_host_ip(item);
                break;
            case PureNet::EAsyncCloseLink:
                on_net_close_link(item);
                break;
            case PureNet::EAsyncSendMsg:
                on_net_send_msg(item);
                break;
            default:
                PureError("PureNetThread work_req async type error {}", int(item->mType));
                mAsyncRespPool.free(item);
                break;
        }
    }
}

void PureNetThread::work_resp() {
    if (mRespQueue.empty()) {
        return;
    }
    mSwapRespMutex.lock();
    mSwapRespQueue.push_back_list(mRespQueue);
    mSwapRespMutex.unlock();
}

void PureNetThread::listen(ESockType type, LinkType key, GroupID groupID, const char* ip, int port, std::function<ListenCallback> cb) {
    auto req = mAsyncReqPool.get();
    NetMsgPtr msg = NetMsg::get();
    auto waitReq = mReqPool.get();
    int err = 0;
    do {
        if (req == nullptr || !msg || waitReq == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        err = PureMsg::pack_args(*msg, int(type), key, groupID, ip, port);
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        req->mReqID = mReqGen.gen_id();
        req->mType = EAsyncListen;
        req->mMsg = msg;
        waitReq->mReqID = req->mReqID;
        waitReq->mReqTime = PureCore::steady_milli_s();
        waitReq->mResp = [=](NetMsgPtr resp) {
            if (!resp) {
                cb(ErrorNullPointer, groupID);
                return;
            }
            int respErr = 0;
            GroupID groupID = 0;
            int err = PureMsg::unpack_args(*resp, respErr, groupID);
            if (err != PureMsg::Success) {
                respErr = err;
            }
            cb(respErr, groupID);
        };
        if (!mReqWaiting.insert(std::make_pair(waitReq->mReqID, waitReq)).second) {
            err = ErrorAddNetReqFailed;
            break;
        }
        mReqQueue.push_back(req);
    } while (false);

    if (err != Success) {
        cb(err, groupID);
        mAsyncReqPool.free(req);
        mReqPool.free(waitReq);
    }
}

void PureNetThread::stop_listen(ESockType type, GroupID groupID) {
    auto req = mAsyncReqPool.get();
    NetMsgPtr msg = NetMsg::get();
    int err = 0;
    do {
        if (req == nullptr || !msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        err = PureMsg::pack_args(*msg, int(type), groupID);
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        req->mType = EAsyncStopListen;
        req->mMsg = msg;
        mReqQueue.push_back(req);
    } while (false);

    if (err != Success) {
        mAsyncReqPool.free(req);
    }
}

void PureNetThread::connect(ESockType type, LinkType key, GroupID groupID, const char* host, int port, std::function<ConnectCallback> cb) {
    auto req = mAsyncReqPool.get();
    NetMsgPtr msg = NetMsg::get();
    auto waitReq = mReqPool.get();
    int err = 0;
    do {
        if (req == nullptr || !msg || waitReq == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        err = PureMsg::pack_args(*msg, int(type), key, groupID, host, port);
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        req->mReqID = mReqGen.gen_id();
        req->mType = EAsyncConnect;
        req->mMsg = msg;
        waitReq->mReqID = req->mReqID;
        waitReq->mReqTime = PureCore::steady_milli_s();
        waitReq->mResp = [=](NetMsgPtr resp) {
            if (!resp) {
                cb(ErrorNullPointer, groupID, 0);
                return;
            }
            int respErr = 0;
            GroupID groupID = 0;
            LinkID linkID = 0;
            int err = PureMsg::unpack_args(*resp, respErr, groupID, linkID);
            if (err != PureMsg::Success) {
                respErr = err;
            }
            cb(respErr, groupID, linkID);
        };
        if (!mReqWaiting.insert(std::make_pair(waitReq->mReqID, waitReq)).second) {
            err = ErrorAddNetReqFailed;
            break;
        }
        mReqQueue.push_back(req);
    } while (false);

    if (err != Success) {
        cb(err, groupID, 0);
        mAsyncReqPool.free(req);
        mReqPool.free(waitReq);
    }
}

void PureNetThread::on_net_listen(AsyncItem* item) {
    int sockType = 0;
    LinkType key = 0;
    GroupID groupID = 0;
    std::string ip;
    int port = 0;
    int err = 0;
    auto resp = mAsyncRespPool.get();
    NetMsgPtr msg = NetMsg::get();
    do {
        if (resp == nullptr || !msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        err = PureMsg::unpack_args(*item->mMsg, sockType, key, groupID, ip, port);
        if (err != PureMsg::Success) {
            err = ErrorUnpackMsgFailed;
            break;
        }
        switch (sockType) {
            case PureNet::ESockTcp: {
                int respErr = mReacter.listen_tcp(key, groupID, ip.c_str(), port);
                err = PureMsg::pack_args(*msg, respErr, groupID);
                if (err != PureMsg::Success) {
                    err = ErrorPackMsgFailed;
                }
                break;
            }
            default:
                err = ErrorInvalidSockType;
                break;
        }

    } while (false);
    if (err == Success) {
        resp->mReqID = item->mReqID;
        resp->mType = item->mType;
        resp->mMsg = msg;
        mRespQueue.push_back(resp);
    } else {
        mAsyncRespPool.free(resp);
        PureError("on_net_listen failed {}", get_error_desc(err));
    }
    mAsyncRespPool.free(item);
}

void PureNetThread::on_net_stop_listen(AsyncItem* item) {
    int sockType = 0;
    GroupID groupID = 0;
    int err = 0;
    do {
        err = PureMsg::unpack_args(*item->mMsg, sockType, groupID);
        if (err != PureMsg::Success) {
            err = ErrorUnpackMsgFailed;
            break;
        }
        switch (sockType) {
            case PureNet::ESockTcp:
                mReacter.stop_listen_tcp(groupID);
                break;
            default:
                err = ErrorInvalidSockType;
                break;
        }
    } while (false);
    if (err != Success) {
        PureError("on_net_stop_listen failed {}", get_error_desc(err));
    }
    mAsyncRespPool.free(item);
}

void PureNetThread::on_net_connect(AsyncItem* item) {
    int sockType = 0;
    LinkType key = 0;
    GroupID groupID = 0;
    std::string host;
    int port = 0;
    int err = 0;
    auto resp = mAsyncRespPool.get();
    NetMsgPtr msg = NetMsg::get();
    do {
        if (resp == nullptr || !msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        err = PureMsg::unpack_args(*item->mMsg, sockType, key, groupID, host, port);
        if (err != PureMsg::Success) {
            err = ErrorUnpackMsgFailed;
            break;
        }
    } while (false);

    if (err == Success) {
        resp->mReqID = item->mReqID;
        resp->mType = item->mType;
        resp->mMsg = msg;
        switch (sockType) {
            case PureNet::ESockTcp: {
                mReacter.connect_tcp(key, groupID, host.c_str(), port, [=](int respErr, GroupID groupID, LinkID linkID) {
                    int err = PureMsg::pack_args(*resp->mMsg, respErr, groupID, linkID);
                    if (err == PureMsg::Success) {
                        mRespQueue.push_back(resp);
                    } else {
                        mAsyncRespPool.free(resp);
                        PureError("on_net_connect tcp pack resp failed");
                    }
                });
                break;
            }
            default:
                err = ErrorInvalidSockType;
                break;
        }

    } else {
        mAsyncRespPool.free(resp);
        PureError("on_net_connect_tcp failed {}", get_error_desc(err));
    }
    mAsyncRespPool.free(item);
}

void PureNetThread::on_net_get_host_ip(AsyncItem* item) {
    std::string host;
    int err = 0;
    auto resp = mAsyncRespPool.get();
    NetMsgPtr msg = NetMsg::get();
    do {
        if (resp == nullptr || !msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        err = PureMsg::unpack_args(*item->mMsg, host);
        if (err != PureMsg::Success) {
            err = ErrorUnpackMsgFailed;
            break;
        }
    } while (false);

    if (err == Success) {
        resp->mReqID = item->mReqID;
        resp->mType = item->mType;
        resp->mMsg = msg;

        mReacter.get_host_ip(host.c_str(), [=](int respErr, const char* ip) {
            int err = PureMsg::pack_args(*resp->mMsg, respErr, ip);
            if (err == PureMsg::Success) {
                mRespQueue.push_back(resp);
            } else {
                mAsyncRespPool.free(resp);
                PureError("on_net_get_host_ip pack resp failed");
            }
        });
    } else {
        mAsyncRespPool.free(resp);
        PureError("on_net_get_host_ip failed {}", get_error_desc(err));
    }
    mAsyncRespPool.free(item);
}

void PureNetThread::on_net_close_link(AsyncItem* item) {
    int reason = 0;
    int err = PureMsg::unpack_args(*item->mMsg, reason);
    if (err == PureMsg::Success) {
        mReacter.link_mgr().close_link(item->mMsg->get_link_id(), reason);
    } else {
        PureError("on_net_close_link failed {}", PureMsg::get_error_desc(err));
    }
    mAsyncRespPool.free(item);
}

void PureNetThread::on_net_send_msg(AsyncItem* item) {
    int err = mReacter.link_mgr().auto_send_msg(item->mMsg);
    if (err != Success) {
        PureError("on_net_send_msg failed {}", get_error_desc(err));
    }
    mAsyncRespPool.free(item);
}

bool PureNetThread::on_link_open(Link* link) {
    if (link == nullptr) {
        return true;
    }
    char ip[INET6_ADDRSTRLEN]{};
    int port = 0;
    int err = 0;
    AsyncItem* resp = nullptr;
    do {
        err = link->get_remote_ip_port(ip, &port);
        if (err != Success) {
            break;
        }
        NetMsgPtr msg = NetMsg::get();
        if (!msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        msg->set_group_id(link->get_group_id());
        msg->set_link_id(link->get_link_id());
        err = PureMsg::pack_args(*msg, ip, port);
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        resp = mAsyncRespPool.get();
        if (resp == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        resp->mReqID = 0;
        resp->mType = EAsyncLinkOpen;
        resp->mMsg = msg;
    } while (false);
    if (err == Success) {
        mRespQueue.push_back(resp);
    } else {
        mAsyncRespPool.free(resp);
        PureError("on_link_open error {}", get_error_desc(err));
    }
    return true;
}

bool PureNetThread::on_link_start(Link* link) {
    if (link == nullptr) {
        return true;
    }
    int err = 0;
    AsyncItem* resp = nullptr;
    do {
        NetMsgPtr msg = NetMsg::get();
        if (!msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        msg->set_group_id(link->get_group_id());
        msg->set_link_id(link->get_link_id());
        resp = mAsyncRespPool.get();
        if (resp == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        resp->mReqID = 0;
        resp->mType = EAsyncLinkStart;
        resp->mMsg = msg;
    } while (false);
    if (err == Success) {
        mRespQueue.push_back(resp);
    } else {
        mAsyncRespPool.free(resp);
        PureError("on_link_start error {}", get_error_desc(err));
    }
    return true;
}

bool PureNetThread::on_link_msg(Link* link) {
    if (link == nullptr) {
        return true;
    }
    NetMsgPtr msg = link->pop_read_msg();
    if (!msg) {
        return true;
    }
    int err = 0;
    AsyncItem* resp = nullptr;
    do {
        msg->set_group_id(link->get_group_id());
        msg->set_link_id(link->get_link_id());
        resp = mAsyncRespPool.get();
        if (resp == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        resp->mReqID = 0;
        resp->mType = EAsyncLinkMsg;
        resp->mMsg = msg;
    } while (false);
    if (err == Success) {
        mRespQueue.push_back(resp);
    } else {
        mAsyncRespPool.free(resp);
        PureError("on_link_msg error {}", get_error_desc(err));
    }
    return true;
}

bool PureNetThread::on_link_end(Link* link) {
    if (link == nullptr) {
        return true;
    }
    int err = 0;
    AsyncItem* resp = nullptr;
    do {
        NetMsgPtr msg = NetMsg::get();
        if (!msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        msg->set_group_id(link->get_group_id());
        msg->set_link_id(link->get_link_id());
        err = PureMsg::pack_args(*msg, link->get_close_reason());
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        resp = mAsyncRespPool.get();
        if (resp == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        resp->mReqID = 0;
        resp->mType = EAsyncLinkEnd;
        resp->mMsg = msg;
    } while (false);
    if (err == Success) {
        mRespQueue.push_back(resp);
    } else {
        mAsyncRespPool.free(resp);
        PureError("on_link_end error {}", get_error_desc(err));
    }
    return true;
}

bool PureNetThread::on_link_close(Link* link) {
    if (link == nullptr) {
        return true;
    }
    int err = 0;
    AsyncItem* resp = nullptr;
    do {
        NetMsgPtr msg = NetMsg::get();
        if (!msg) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        msg->set_group_id(link->get_group_id());
        msg->set_link_id(link->get_link_id());
        err = PureMsg::pack_args(*msg, link->get_close_reason());
        if (err != PureMsg::Success) {
            err = ErrorPackMsgFailed;
            break;
        }
        resp = mAsyncRespPool.get();
        if (resp == nullptr) {
            err = ErrrorMemoryNotEnough;
            break;
        }
        resp->mReqID = 0;
        resp->mType = EAsyncLinkClose;
        resp->mMsg = msg;
    } while (false);
    if (err == Success) {
        mRespQueue.push_back(resp);
    } else {
        mAsyncRespPool.free(resp);
        PureError("on_link_close error {}", get_error_desc(err));
    }
    return true;
}
}  // namespace PureNet
