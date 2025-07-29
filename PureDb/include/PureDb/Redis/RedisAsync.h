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

#pragma once

#include "PureCore/NodeList.h"
#include "PureMsg/MsgBuffer.h"
#include "PureDb/PureDbLib.h"
#include "PureDb/Redis/RedisReply.h"

#include <stdint.h>
#include <functional>

namespace PureDb {
enum ERedisAsyncType {
    ERedisAsyncInvalid = 0,
    ERedisAsyncConnect,
    ERedisAsyncCommand,
    ERedisAsyncPipe,
};

class PUREDB_API RedisAsyncItem : public PureCore::Node {
public:
    void clear() {
        mReqID = 0;
        mType = ERedisAsyncInvalid;
        mMsg.clear();
        mReply.remove();
    }

    int64_t mReqID{};
    ERedisAsyncType mType{};
    PureMsg::MsgDynamicBuffer mMsg{};
    RedisReplyPtr mReply;
};

struct PUREDB_API RedisReqItem {
    int64_t mReqID{};
    int64_t mReqTime{};
    std::function<void(RedisReplyPtr)> mResp{};
};

}  // namespace PureDb
