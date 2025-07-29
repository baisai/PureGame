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

#include "PureEncrypt/PureCrc32.h"
#include "PureNet/NetErrorDesc.h"
#include "PureNet/LinkFactory.h"
#include "PureNet/PureNetReacter.h"
#include "PureNet/PureNetReq.h"
#include "PureNet/LinkTcp.h"

namespace PureNet {
enum EReacterState {
    EReacterInvalid = 0,
    EReacterValid = 1,
    EReacterClosing = 2,
};

PureNetReacter::PureNetReacter() {}

const NetConfig& PureNetReacter::config() const { return mConfig; }

void PureNetReacter::set_config(const NetConfig& cfg) { mConfig = cfg; }

int PureNetReacter::init() {
    if (mState != EReacterInvalid) {
        return ErrorStateError;
    }
    int err = mLinks.init();
    if (err != Success) {
        return err;
    }
    err = uv_loop_init(&mLoop);
    if (err != 0) {
        return err;
    }
    mLoop.data = this;
    mTimer.init();
    mState = EReacterValid;
    return Success;
}

void PureNetReacter::release() {
    if (mState != EReacterValid) {
        return;
    }
    mState = EReacterClosing;
    for (auto iter = mListenTcp.begin(); iter != mListenTcp.end(); ++iter) {
        stop_listen_tcp_req(iter->cast<ListenTcpReq>());
    }

    mLinks.close_all_link(Success);
    while (uv_loop_alive(&mLoop)) {
        update(0);
    }

    mTimer.release();
    int err = uv_loop_close(&mLoop);
    if (err != 0) {
        PureError("PureNetReacter release close loop error {}", get_error_desc(err));
    }
    while (!mListenTcp.empty()) {
        delete mListenTcp.pop_front_t<ListenTcpReq>();
    }

    mReadyFrame.clear();
    mWorkFrame.clear();
    mEventLinkOpen.clear();
    mEventLinkStart.clear();
    mEventLinkMsg.clear();
    mEventLinkEnd.clear();
    mEventLinkClose.clear();
    mLinks.release();
    mLoop.data = nullptr;
    mState = EReacterInvalid;
}

LinkMgr& PureNetReacter::link_mgr() { return mLinks; }

uv_loop_t& PureNetReacter::get_uv_handle() { return mLoop; }

PureCore::RandomGen& PureNetReacter::randomer() { return mRandGen; }

PureCore::TWTimer& PureNetReacter::timer() { return mTimer; }

void PureNetReacter::update(int64_t delta) {
    if (mState == EReacterInvalid) {
        return;
    }
    mTimer.update(delta);
    mLinks.flush_link();
    uv_run(&mLoop, UV_RUN_NOWAIT);
    for (auto& iter : mWorkFrame) {
        iter();
    }
    mWorkFrame.clear();
    mReadyFrame.swap(mWorkFrame);
}

int PureNetReacter::listen_tcp(LinkType key, GroupID groupID, const char* ip, int port) {
    if (mState != EReacterValid) {
        return ErrorStateError;
    }
    SockAddr addr;
    int err = set_ip_port_to_addr(addr, ip, port);
    if (err != Success) {
        return err;
    }
    ListenTcpReq* info = new ListenTcpReq(groupID, [=](uv_stream_t* server, int status) {
        if (status != 0) {
            PureError(get_error_desc(status));
            return;
        }
        int err = on_accept_tcp(key, server);
        if (err != Success) {
            PureError(get_error_desc(err));
        }
    });
    if (info == nullptr) {
        return ErrrorMemoryNotEnough;
    }
    if ((err = uv_tcp_init(&get_uv_handle(), &info->mHandle)) || (err = uv_tcp_bind(&info->mHandle, &addr.addr, 0)) ||
        (err = uv_listen((uv_stream_t*)(&info->mHandle), SOMAXCONN, [](uv_stream_t* server, int status) {
             if (server == nullptr || server->data == nullptr) {
                 return;
             }
             ListenTcpReq* self = (ListenTcpReq*)server->data;
             self->mAccept(server, status);
         }))) {
        uv_close((uv_handle_t*)(&info->mHandle), [](uv_handle_t* handle) {
            if (handle == nullptr || handle->data == nullptr) {
                return;
            }
            ListenTcpReq* self = (ListenTcpReq*)handle->data;
            delete self;
        });
        return err;
    }
    mListenTcp.push_back(info);
    return Success;
}

void PureNetReacter::stop_listen_tcp(GroupID groupID) {
    if (mState != EReacterValid) {
        return;
    }
    for (auto iter = mListenTcp.begin(); iter != mListenTcp.end(); ++iter) {
        ListenTcpReq* pNode = iter->cast<ListenTcpReq>();
        if (pNode->mGroupID == groupID) {
            stop_listen_tcp_req(pNode);
        }
    }
}

void PureNetReacter::connect_tcp(LinkType key, GroupID groupID, const char* host, int port, std::function<ConnectCallback> cb) {
    if (!cb) {
        PureError("tcp connect cb is nullptr");
        return;
    }
    if (host == nullptr || port == 0) {
        cb(ErrorInvalidArg, 0, 0);
        return;
    }
    if (mState != EReacterValid) {
        cb(ErrorStateError, 0, 0);
        return;
    }
    get_host_addr(host, [=](int err, const sockaddr* addr) {
        if (err != Success) {
            cb(err, 0, 0);
            return;
        }
        SockAddr fullAddr;
        err = set_addr_port(fullAddr, addr, port);
        if (err != Success) {
            cb(err, 0, 0);
            return;
        }
        Link* link = LinkFactory::get(key);
        if (link == nullptr) {
            cb(ErrrorMemoryNotEnough, 0, 0);
            return;
        }
        err = link->init(this);
        if (err != Success) {
            link->free();
            cb(err, 0, 0);
            return;
        }

        err = mLinks.add_link(link, groupID, false);
        if (err != Success) {
            mLinks.close_link(link, err);
            cb(err, 0, 0);
            return;
        }
        err = connect_tcp_link(static_cast<LinkTcp*>(link), &fullAddr.addr, cb);
        if (err != Success) {
            mLinks.close_link(link, err);
            cb(err, 0, 0);
            return;
        }
    });
}

void PureNetReacter::close_link(LinkID linkID, int reason) {
    if (mState != EReacterValid) {
        return;
    }
    mLinks.close_link(linkID, reason);
}

void PureNetReacter::get_host_ip(const char* host, std::function<GetHostIpCallback> cb) {
    get_host_addr(host, [=](int err, const sockaddr* addr) {
        if (err != Success) {
            cb(err, "");
            return;
        }
        char ip[INET6_ADDRSTRLEN] = {};
        err = uv_ip_name(addr, ip, INET6_ADDRSTRLEN);
        cb(err, ip);
    });
}

void PureNetReacter::add_next_frame(const std::function<void()>& cb) {
    if (mState == EReacterInvalid) {
        return;
    }
    mReadyFrame.push_back(cb);
}

void PureNetReacter::close_tcp(LinkTcp* link, int reason) {
    if (link == nullptr) {
        return;
    }
    uv_tcp_t& handle = link->get_uv_handle();
    if (handle.loop == nullptr || handle.loop->data == nullptr) {
        return;
    }

    if (uv_is_writable((const uv_stream_t*)&handle)) {
        uv_shutdown_t* sreq = get_shutdown_t();
        sreq->data = this;
        int err = uv_shutdown(sreq, (uv_stream_t*)&handle, [](uv_shutdown_t* req, int status) {
            if (req == nullptr || req->data == nullptr) {
                return;
            }
            PureNetReacter* self = (PureNetReacter*)req->data;
            self->on_tcp_shutdown(req, status);
        });
        if (err == 0) {
            return;
        }

        free_shutdown_t(sreq);
    }
    if (uv_is_closing((const uv_handle_t*)&handle)) {
        return;
    }
    uv_close((uv_handle_t*)&handle, [](uv_handle_t* peer) {
        if (peer == nullptr || peer->data == nullptr || peer->loop == nullptr || peer->loop->data == nullptr) {
            return;
        }
        PureNetReacter* self = (PureNetReacter*)peer->loop->data;
        Link* link = (Link*)peer->data;
        self->on_close(link);
    });
}

uv_shutdown_t* PureNetReacter::get_shutdown_t() { return mPools.mShutdownPool.get(); }

void PureNetReacter::free_shutdown_t(uv_shutdown_t* obj) { return mPools.mShutdownPool.free(obj); }

GetAddrReq* PureNetReacter::get_getaddr_req() { return mPools.mGetAddrPool.get(); }

void PureNetReacter::free_getaddr_req(GetAddrReq* obj) { mPools.mGetAddrPool.free(obj); }

ConnectTcpReq* PureNetReacter::get_connect_tcp_req() { return mPools.mTcpConnectPool.get(); }

void PureNetReacter::free_connect_tcp_req(ConnectTcpReq* obj) { mPools.mTcpConnectPool.free(obj); }

PureCore::FixedBuffer* PureNetReacter::get_tcp_buffer() { return mPools.mTcpBufferPool.get(mConfig.mTcpBufferSize); }

void PureNetReacter::free_tcp_buffer(PureCore::FixedBuffer* obj) { mPools.mTcpBufferPool.free(obj); }

WriteTcpReq* PureNetReacter::get_tcp_write_req() { return mPools.mWriteTcpPool.get(); }

void PureNetReacter::free_tcp_write_req(WriteTcpReq* obj) { mPools.mWriteTcpPool.free(obj); }

int PureNetReacter::connect_tcp_link(LinkTcp* link, const struct sockaddr* addr, std::function<ConnectCallback> cb) {
    if (link == nullptr || addr == nullptr) {
        return ErrorNullPointer;
    }
    auto req = get_connect_tcp_req();
    if (req == nullptr) {
        return ErrrorMemoryNotEnough;
    }
    int err = req->init(link, cb);
    if (err != Success) {
        free_connect_tcp_req(req);
        return err;
    }
    err = uv_tcp_connect(&req->mHandle, (uv_tcp_t*)&link->get_uv_handle(), addr, [](uv_connect_t* con, int status) {
        if (con == nullptr || con->data == nullptr || con->handle == nullptr || con->handle->loop == nullptr || con->handle->loop->data == nullptr) {
            PureError("tcp connect callback failed, handle is nullptr");
            return;
        }
        int err = status;
        auto req = (ConnectTcpReq*)con->data;
        PureNetReacter* self = (PureNetReacter*)con->handle->loop->data;
        do {
            if (err != Success) {
                break;
            }
            if (req->mLink == nullptr) {
                PureError("tcp connect callback failed, link is nullptr");
                err = ErrorNotFoundLink;
                break;
            }
            err = self->open_tcp_link(req->mLink);
            if (err != Success) {
                break;
            }
        } while (false);

        req->mConnected(err, req->mLink->get_group_id(), req->mLink->get_link_id());
        if (err != Success) {
            self->mLinks.close_link(req->mLink, err);
        }
        self->free_connect_tcp_req(req);
    });
    if (err != 0) {
        free_connect_tcp_req(req);
    }
    return err;
}

void PureNetReacter::get_host_addr(const char* host, std::function<GetHostAddrCallback> cb) {
    if (!cb) {
        PureError("get host addr failed, cb is nullptr");
        return;
    }
    if (host == nullptr) {
        cb(ErrorInvalidArg, nullptr);
        return;
    }
    if (mState != EReacterValid) {
        cb(ErrorStateError, nullptr);
        return;
    }
    GetAddrReq* req = get_getaddr_req();
    if (req == nullptr) {
        cb(ErrrorMemoryNotEnough, nullptr);
        return;
    }
    int err = req->init(cb);
    if (err != Success) {
        cb(err, nullptr);
        free_getaddr_req(req);
        return;
    }
    err = uv_getaddrinfo(
        &mLoop, &req->mHandle,
        [](uv_getaddrinfo_t* h, int status, addrinfo* res) {
            if (h == nullptr || h->data == nullptr || h->loop == nullptr || h->loop->data == nullptr) {
                PureError("get host addr failed, req is nullptr");
                return;
            }
            GetAddrReq* req = (GetAddrReq*)h->data;
            if (status == 0 && res != nullptr && res->ai_addr != nullptr) {
                req->mGetted(Success, res->ai_addr);
            } else {
                if (status == 0) {
                    status = ErrorNullPointer;
                }
                req->mGetted(status, nullptr);
            }
            PureNetReacter* self = (PureNetReacter*)h->loop->data;
            self->free_getaddr_req(req);
            if (res != nullptr) {
                uv_freeaddrinfo(res);
            }
        },
        host, nullptr, nullptr);
    if (err != 0) {
        free_getaddr_req(req);
        cb(err, nullptr);
    }
}

void PureNetReacter::stop_listen_tcp_req(ListenTcpReq* req) {
    if (req == nullptr) {
        return;
    }
    uv_close((uv_handle_t*)(&req->mHandle), [](uv_handle_t* handle) {
        if (handle == nullptr || handle->data == nullptr) {
            return;
        }
        ListenTcpReq* self = (ListenTcpReq*)handle->data;
        delete self;
    });
}

int PureNetReacter::on_accept_tcp(LinkType key, uv_stream_t* server) {
    if (server == nullptr || server->data == nullptr) {
        return ErrorNullPointer;
    }
    LinkTcp* link = static_cast<LinkTcp*>(LinkFactory::get(key));
    if (link == nullptr) {
        return ErrrorMemoryNotEnough;
    }
    int err = link->init(this);
    if (err != Success) {
        link->free();
        return err;
    }

    ListenTcpReq* req = (ListenTcpReq*)(server->data);
    err = mLinks.add_link(link, req->mGroupID, true);
    if (err != Success) {
        mLinks.close_link(link, err);
        return err;
    }
    err = uv_accept(server, (uv_stream_t*)&link->get_uv_handle());
    if (err != 0) {
        mLinks.close_link(link, err);
        return err;
    }

    err = open_tcp_link(link);
    if (err != Success) {
        mLinks.close_link(link, err);
        return err;
    }
    return Success;
}

void PureNetReacter::on_read_alloc_tcp(LinkTcp* link, uv_buf_t* buf) {
    if (link == nullptr || link->get_reader() == nullptr || buf == nullptr) {
        return;
    }
    auto freeBuf = link->get_reader()->free_buffer();
    buf->base = freeBuf.data();
    buf->len = static_cast<uint32_t>(freeBuf.size());
}

void PureNetReacter::on_read_tcp(LinkTcp* link, ssize_t nread, const uv_buf_t* buf) {
    if (link == nullptr || link->get_reader() == nullptr || nread == 0) {
        return;
    }
    if (nread < 0) {
        mLinks.close_link(link, int(nread));
        return;
    }
    link->get_reader()->write_pos(link->get_reader()->write_pos() + nread);
    int err = link->read();
    if (err != Success) {
        mLinks.close_link(link, err);
        PureError("on_read_tcp failed, error `{}`", get_error_desc(err));
        return;
    }
    link->get_reader()->clear();
}

int PureNetReacter::open_tcp_link(LinkTcp* link) {
    if (link == nullptr) {
        return ErrorNullPointer;
    }
    int err = uv_read_start((uv_stream_t*)&link->get_uv_handle(),
                            [](uv_handle_t* handle, size_t, uv_buf_t* buf) {
                                if (handle == nullptr || handle->data == nullptr) {
                                    PureError("alloc read cb failed, handle is nullptr");
                                    return;
                                }
                                LinkTcp* link = static_cast<LinkTcp*>((Link*)handle->data);
                                PureNetReacter* self = link->reacter();
                                if (self == nullptr) {
                                    PureError("alloc read cb failed, reacter is nullptr");
                                    return;
                                }
                                self->on_read_alloc_tcp(link, buf);
                            },
                            [](uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
                                if (handle == nullptr || handle->data == nullptr) {
                                    PureError("read cb failed, handle is nullptr");
                                    return;
                                }
                                LinkTcp* link = static_cast<LinkTcp*>((Link*)handle->data);
                                PureNetReacter* self = link->reacter();
                                if (self == nullptr) {
                                    PureError("read cb failed, reacter is nullptr");
                                    return;
                                }
                                self->on_read_tcp(link, nread, buf);
                            });
    if (err != 0) {
        return err;
    }
    link->on_open();
    return Success;
}

void PureNetReacter::on_tcp_shutdown(uv_shutdown_t* req, int status) {
    uv_close((uv_handle_t*)req->handle, [](uv_handle_t* peer) {
        if (peer == nullptr || peer->data == nullptr || peer->loop == nullptr || peer->loop->data == nullptr) {
            return;
        }
        PureNetReacter* self = (PureNetReacter*)peer->loop->data;
        Link* link = (Link*)peer->data;
        self->on_close(link);
    });
    free_shutdown_t(req);
}

void PureNetReacter::on_close(Link* link) {
    if (link == nullptr) {
        return;
    }
    link->on_close();
    add_next_frame([=]() {
        mLinks.remove_link(link->get_link_id());
        link->free();
    });
}

void PureNetReacter::on_link_open(Link* link) {
    mEventLinkOpen.notify(link);
    int64_t keepAlive = config().mKeepAlive;
    if (keepAlive > 0) {
        int64_t timerID = mTimer.add_timer(
            1, keepAlive, keepAlive, -1,
            [=](int64_t timerID, int32_t timerType, int64_t leftTimes) {
                if (!link->is_alive()) {
                    link->set_alive_timer(0);
                    mLinks.close_link(link, ErrorLinkKeepAliveTimeout);
                    return false;
                }
                return true;
            },
            true);
        if (timerID <= 0) {
            PureError("on_link_open ({}:{}) add keep alive timer failed", link->get_group_id(), link->get_link_id());
            mLinks.close_link(link, ErrorLinkKeepAliveFailed);
        } else {
            link->set_alive_timer(timerID);
        }
    }
}

void PureNetReacter::on_link_start(Link* link) { mEventLinkStart.notify(link); }

void PureNetReacter::on_link_msg(Link* link) { mEventLinkMsg.notify(link); }

void PureNetReacter::on_link_end(Link* link) { mEventLinkEnd.notify(link); }

void PureNetReacter::on_link_close(Link* link) {
    mEventLinkClose.notify(link);
    if (link->get_alive_timer() > 0) {
        mTimer.remove_timer(link->get_alive_timer());
        link->set_alive_timer(0);
    }
}

}  // namespace PureNet
