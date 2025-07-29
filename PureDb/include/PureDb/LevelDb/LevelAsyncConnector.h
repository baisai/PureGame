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

#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureCore/Thread.h"
#include "PureCore/NodeList.h"
#include "PureCore/IncrIDGen.h"
#include "PureCore/Memory/ObjectPool.h"
#include "PureCore/Memory/ObjectCache.h"
#include "PureDb/PureDbLib.h"
#include "PureDb/LevelDb/LevelConnector.h"
#include "PureDb/LevelDb/LevelAsync.h"

#include <mutex>

namespace PureDb {
class PUREDB_API LevelAsyncConnector : public PureCore::Thread {
public:
    LevelAsyncConnector() = default;
    virtual ~LevelAsyncConnector() = default;

    // logic
    void stop();

    // logic
    int init(int64_t reqTimeout);
    void update();

    // logic
    int connect(std::function<void(LevelReplyPtr)> cb, const LevelConfig& cfg);

    int vset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef data);
    int vdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int vget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int vmset(std::function<void(LevelReplyPtr)> cb, PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas);
    int vmdel(std::function<void(LevelReplyPtr)> cb, PureCore::ArrayRef<PureCore::StringRef> names);
    int vmget(std::function<void(LevelReplyPtr)> cb, PureCore::ArrayRef<PureCore::StringRef> names);
    int v_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int create_v_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef start, PureCore::StringRef end, bool forward = true);

    // name not \0 safe
    int hset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key, PureCore::StringRef data);
    int hdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key);
    int hget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key);
    int hmset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas);
    int hmdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int hmget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int hgetall(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int hdrop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int h_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int create_h_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end,
                          bool forward = true);

    // name not \0 safe
    int sset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key);
    int sdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key);
    int sget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef key);
    int smset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int smdel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int sgetall(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int sdrop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int s_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int create_s_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end,
                          bool forward = true);

    // name not \0 safe
    int lset(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t idx, PureCore::StringRef data);
    int llpush(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef data);
    int llpop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int lrpush(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, PureCore::StringRef data);
    int lrpop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int lget(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t idx);
    int lrange(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t start, int64_t end);
    int ldel(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t count, PureCore::StringRef data);
    int ltrim(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, int64_t start, int64_t end);
    int lgetall(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int ldrop(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int l_approximate_size(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int create_l_iterator(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name, bool forward = true);

    int approximate_size(std::function<void(LevelReplyPtr)> cb);
    int property(std::function<void(LevelReplyPtr)> cb, PureCore::StringRef name);
    int compact();

    // batch < 0, all
    int backup_to(std::function<void(LevelReplyPtr)> cb, const char* path, int64_t batch, ELevelBackupCompress compress = ELevelCompressLz4);

    int iterator(std::function<void(LevelReplyPtr)> cb, int64_t id, size_t count, bool forward = true);
    int remove_iterator(std::function<void(LevelReplyPtr)> cb, int64_t id);

private:
    // logic
    void logic_req();
    void logic_resp();
    int make_req(ELevelAsyncType reqType, std::function<void(LevelReplyPtr)> cb, std::function<int(PureMsg::MsgDynamicBuffer& buffer)> packer);

    // work
    void work_req();
    void work_resp();

protected:
    virtual void work();

private:
    LevelConnector mConnector;
    std::atomic<bool> mRunning{};

    PureCore::NodeList mReqQueue;
    PureCore::NodeList mRespQueue;

    std::mutex mSwapReqMutex;
    PureCore::NodeList mSwapReqQueue;
    std::mutex mSwapRespMutex;
    PureCore::NodeList mSwapRespQueue;

    // logic
    int64_t mReqTimeOut{};
    PureCore::IncrIDGen mReqGen;
    std::map<int64_t, LevelReqItem*> mReqWaiting{};

    // logic
    PureCore::ObjectPool<LevelReqItem, 255> mReqPool;
    PureCore::ObjectCache<LevelAsyncItem, 255> mAsyncItemPool;
};
}  // namespace PureDb
