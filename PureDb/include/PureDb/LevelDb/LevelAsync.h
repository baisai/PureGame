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
#include "PureMsg/MsgClass.h"
#include "PureDb/PureDbLib.h"
#include "PureDb/LevelDb/LevelReply.h"

#include <stdint.h>
#include <functional>

namespace PureDb {
enum ELevelAsyncType {
    ELevelAsyncInvalid = 0,

    ELevelAsyncConnect,

    ELevelAsyncVset,
    ELevelAsyncVdel,
    ELevelAsyncVget,
    ELevelAsyncVmset,
    ELevelAsyncVmdel,
    ELevelAsyncVmget,
    ELevelAsyncVApproximateSize,

    ELevelAsyncHset,
    ELevelAsyncHdel,
    ELevelAsyncHget,
    ELevelAsyncHmset,
    ELevelAsyncHmdel,
    ELevelAsyncHmget,
    ELevelAsyncHgetall,
    ELevelAsyncHdrop,
    ELevelAsyncHApproximateSize,

    ELevelAsyncSset,
    ELevelAsyncSdel,
    ELevelAsyncSget,
    ELevelAsyncSmset,
    ELevelAsyncSmdel,
    ELevelAsyncSgetall,
    ELevelAsyncSdrop,
    ELevelAsyncSApproximateSize,

    ELevelAsyncLset,
    ELevelAsyncLlpush,
    ELevelAsyncLlpop,
    ELevelAsyncLrpush,
    ELevelAsyncLrpop,
    ELevelAsyncLget,
    ELevelAsyncLrange,
    ELevelAsyncLdel,
    ELevelAsyncLtrim,
    ELevelAsyncLgetall,
    ELevelAsyncLdrop,
    ELevelAsyncLApproximateSize,

    ElevelAsyncApproximateSize,
    ELevelAsyncProperty,
    ELevelAsyncCompact,
    ELevelAsyncBackupTo,

    ELevelAsyncIterCreateV,
    ELevelAsyncIterCreateH,
    ELevelAsyncIterCreateS,
    ELevelAsyncIterCreateL,
    ELevelAsyncIterVist,
    ELevelAsyncIterClear,
};

class PUREDB_API LevelAsyncItem : public PureCore::Node {
public:
    void clear() {
        mReqID = 0;
        mType = ELevelAsyncInvalid;
        mMsg.clear();
        mReply.remove();
    }

    int64_t mReqID{};
    ELevelAsyncType mType{};
    PureMsg::MsgDynamicBuffer mMsg{};
    LevelReplyPtr mReply;
};

struct PUREDB_API LevelReqItem {
    int64_t mReqID{};
    int64_t mReqTime{};
    std::function<void(LevelReplyPtr)> mResp{};
};

}  // namespace PureDb
