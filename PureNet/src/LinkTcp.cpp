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

#include "PureNet/NetErrorDesc.h"
#include "PureNet/LinkTcp.h"
#include "PureNet/ProtocolStack.h"
#include "PureNet/PureNetReacter.h"

namespace PureNet {
LinkTcp::LinkTcp(ProtocolStack& ps) : Link(ps) {}

LinkTcp::~LinkTcp() {
    if (mReacter == nullptr) {
        return;
    }
    if (mReader != nullptr) {
        mReacter->free_tcp_buffer(mReader);
    }
    if (mWriter != nullptr) {
        mReacter->free_tcp_buffer(mWriter);
    }
}

int LinkTcp::init(PureNetReacter* reacter) {
    int err = Link::init(reacter);
    if (err != Success) {
        return err;
    }
    if (mReader == nullptr) {
        mReader = mReacter->get_tcp_buffer();
    } else {
        mReader->clear();
    }
    if (mWriter == nullptr) {
        mWriter = mReacter->get_tcp_buffer();
    } else {
        mWriter->clear();
    }
    if (mReader == nullptr || mWriter == nullptr) {
        return ErrrorMemoryNotEnough;
    }
    return mHandle.init(reacter, this);
}

void LinkTcp::clear() {
    if (mReacter != nullptr) {
        mReacter->free_tcp_buffer(mReader);
        mReacter->free_tcp_buffer(mWriter);
    }
    Link::clear();
    mHandle.release();
}

uv_tcp_t& LinkTcp::get_uv_handle() { return mHandle.get_uv_handle(); }

int LinkTcp::get_remote_ip_port(char* ip, int* port) { return mHandle.get_remote_ip_port(ip, port); }

int LinkTcp::get_local_ip_port(char* ip, int* port) { return mHandle.get_local_ip_port(ip, port); }

int LinkTcp::flush_data() {
    if (!valid() || mWriter == nullptr) {
        return ErrorStateError;
    }
    auto data = mWriter->data();
    if (data.empty()) {
        return Success;
    }
    auto req = mReacter->get_tcp_write_req();
    if (req == nullptr) {
        return ErrrorMemoryNotEnough;
    }

    int err = 0;
    mWriter->read_pos(mWriter->read_pos() + data.size());
    if (mWriter->free_size() == 0) {
        err = req->init(this, data, swap_writer(mReacter->get_tcp_buffer()));
    } else {
        err = req->init(this, data, nullptr);
    }
    if (err != Success) {
        mReacter->free_tcp_write_req(req);
        return err;
    }
    add_writing_size(data.size());
    err = uv_write(&req->mHandle, (uv_stream_t*)&get_uv_handle(), &req->mUvBuf, 1, [](uv_write_t* handle, int status) {
        if (handle == nullptr || handle->data == nullptr) {
            PureError("link tcp push data failed, handle is nullptr");
            return;
        }
        WriteTcpReq* req = (WriteTcpReq*)handle->data;
        if (req->mLastBuffer != nullptr) {
            req->mLink->reacter()->free_tcp_buffer(req->mLastBuffer);
        }
        req->mLink->finish_writing_size(req->mUvBuf.len);
        if (req->mLink->get_writing_size() == 0 && req->mLink->get_writer() != nullptr && req->mLink->get_writer()->size() == 0) {
            req->mLink->get_writer()->clear();
        }
        if (status != 0) {
            req->mLink->close(status);
        }
        req->mLink->reacter()->free_tcp_write_req(req);
    });
    if (err != 0) {
        mReacter->free_tcp_write_req(req);
    }
    return err;
}

int LinkTcp::push_data(PureCore::IBuffer& buffer, bool msgEnd) {
    if (!valid() || mWriter == nullptr) {
        return ErrorStateError;
    }
    size_t freeSize = mWriter->free_size();
    if (freeSize >= buffer.size()) {
        int err = mWriter->write(buffer.data());
        if (err != PureCore::Success) {
            return ErrorLinkWriteDataFailed;
        }
    } else {
        PureCore::DataRef data(buffer.data().data(), freeSize);
        int err = mWriter->write(data);
        if (err != PureCore::Success) {
            return ErrorLinkWriteDataFailed;
        }
        buffer.read_pos(buffer.read_pos() + freeSize);
        err = flush_data();
        if (err != PureCore::Success) {
            return ErrorLinkWriteDataFailed;
        }
        return push_data(buffer, msgEnd);
    }
    return Success;
}

int LinkTcp::close(int reason) {
    int err = Link::close(reason);
    if (err != Success) {
        return err;
    }
    mReacter->close_tcp(this, reason);
    return Success;
}

}  // namespace PureNet
