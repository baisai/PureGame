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
#include "PureNet/Protocol/MsgProtocol.h"
#include "PureNet/NetMsg.h"
#include "PureNet/Link.h"
#include "PureNet/PureNetReacter.h"

namespace PureNet {
int MsgProtocol::start(Link* l) {
    mNeedSize = 0;
    if (next() == nullptr) {
        return Success;
    }
    return next()->start(l);
}

int MsgProtocol::read(Link* l, PureCore::IBuffer& buffer) {
    int err = read_to_msg(l, buffer);
    if (err != Success) {
        mReading.remove();
    }
    return err;
}

int MsgProtocol::write_msg(Link* l, NetMsg& msg) {
    if (pre() == nullptr) {
        return ErrorNullPointer;
    }

    if (msg.get_body_flag() != EBodyMsg) {
        return ErrorProtocolDataInvalid;
    }
    return pre()->write(l, msg, 0, msg.size());
}

int MsgProtocol::end(Link* l) {
    if (pre() == nullptr) {
        return Success;
    }
    return pre()->end(l);
}

int MsgProtocol::read_to_msg(Link* l, PureCore::IBuffer& buffer) {
    if (next() == nullptr) {
        return ErrorNullPointer;
    }
    if (!mReading) {
        mNeedSize = 0;
        mReading = NetMsg::get();
        if (!mReading) {
            return ErrrorMemoryNotEnough;
        }
        auto data = buffer.data();
        if (data.size() > sizeof(uint32_t) + 1) {
            data.reset(data.data(), sizeof(uint32_t) + 1);
        }
        mReading->write(data);
        buffer.read_pos(buffer.read_pos() + data.size());
    }
    if (mNeedSize == 0) {
        int err = PureMsg::unpack_bin(*mReading, mNeedSize);
        if (err == PureMsg::ErrorReadBufferFailed) {
            return Success;  // need data
        }
        if (err != PureMsg::Success) {
            return ErrorProtocolDataInvalid;
        }
    }

    if (mNeedSize > 0) {
        if (mReading->size() > mNeedSize) {
            size_t backSize = mReading->size() - mNeedSize;
            mReading->write_pos(mReading->write_pos() - backSize);
            buffer.read_pos(buffer.read_pos() - backSize);
            mNeedSize = 0;
        } else {
            mNeedSize -= uint32_t(mReading->size());
        }
        auto data = buffer.data();
        if (mNeedSize < int64_t(data.size())) {
            data.reset(data.data(), mNeedSize);
        }
        int err = mReading->write(data);
        if (err != PureMsg::Success) {
            return ErrorPackMsgFailed;
        }
        buffer.read_pos(buffer.read_pos() + data.size());
        mNeedSize -= uint32_t(data.size());
    }
    if (mNeedSize == 0) {
        mReading->read_pos(0);
        mReading->set_body_flag(EBodyMsg);
        int err = next()->read_msg(l, mReading);
        if (err != Success) {
            return err;
        }
        if (buffer.size() > 0) {
            return read(l, buffer);
        }
    }
    return Success;
}

}  // namespace PureNet
