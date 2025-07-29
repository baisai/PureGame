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
#include "PureCore/ArrayRef.h"
#include "PureCore/Thread.h"
#include "PureCore/NodeList.h"
#include "PureCore/IncrIDGen.h"
#include "PureCore/Memory/ObjectPool.h"
#include "PureCore/Memory/ObjectCache.h"
#include "PureDb/PureDbLib.h"
#include "PureDb/Redis/RedisConnector.h"
#include "PureDb/Redis/RedisAsync.h"

#include <mutex>

namespace PureDb {
class PUREDB_API RedisAsyncConnector : public PureCore::Thread {
public:
    RedisAsyncConnector() = default;
    virtual ~RedisAsyncConnector() = default;

    // logic
    void stop();

    // logic
    int init(int64_t reqTimeout);
    void update();

    // thread safe
    bool is_busying() const;

    // logic
    int connect(std::function<void(RedisReplyPtr)> cb, const RedisConfig& cfg);
    int add_pipe(PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params);
    template <typename... Params>
    int add_pipe_t(PureCore::StringRef cmd, Params&&... params) {
        int err = req_cmd(cmd, sizeof...(params));
        if (err != Success) {
            return err;
        }
        return req_params(std::forward<Params>(params)...);
    }
    int commit_pipe(std::function<void(RedisReplyPtr)> cb);
    void cancel_pipe();
    int command(std::function<void(RedisReplyPtr)> cb, PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params);
    template <typename... Params>
    int command_t(std::function<void(RedisReplyPtr)> cb, PureCore::StringRef cmd, Params&&... params) {
        cancel_pipe();
        int err = req_cmd(cmd, sizeof...(params));
        if (err != Success) {
            return err;
        }
        err = req_params(std::forward<Params>(params)...);
        if (err != Success) {
            return err;
        }
        return req_commit(cb, false);
    }

private:
    // logic
    int req_cmd(PureCore::StringRef cmd, size_t count);
    int req_param(PureCore::StringRef param);
    int req_params();
    template <typename Param1, typename... Params>
    int req_params(const Param1& param1, Params&&... params) {
        int err = req_param(param1);
        if (err != Success) {
            return err;
        }
        return req_params(std::forward<Params>(params)...);
    }
    int req_commit(std::function<void(RedisReplyPtr)> cb, bool isPipe);

private:
    // logic
    void logic_req();
    void logic_resp();

    // work
    void work_req();
    void work_resp();

protected:
    virtual void work();

private:
    RedisConnector mConnector;
    std::atomic<bool> mRunning{};
    std::atomic<bool> mBusying{};

    PureCore::NodeList mReqQueue;
    PureCore::NodeList mRespQueue;

    std::mutex mSwapReqMutex;
    PureCore::NodeList mSwapReqQueue;
    std::mutex mSwapRespMutex;
    PureCore::NodeList mSwapRespQueue;

    // work
    std::vector<PureCore::StringRef> mParamsCache;

    // logic
    int64_t mReqTimeOut{};
    PureCore::IncrIDGen mReqGen;
    std::map<int64_t, RedisReqItem*> mReqWaiting{};
    RedisAsyncItem* mReqing = nullptr;

    // logic
    PureCore::ObjectPool<RedisReqItem, 255> mReqPool;
    PureCore::ObjectCache<RedisAsyncItem, 255> mAsyncItemPool;
};
}  // namespace PureDb
