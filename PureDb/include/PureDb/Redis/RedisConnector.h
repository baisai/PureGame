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

#include "PureCore/StringRef.h"
#include "PureCore/ArrayRef.h"
#include "PureMsg/MsgClass.h"
#include "PureDb/PureDbLib.h"
#include "PureDb/DbErrorDesc.h"
#include "PureDb/Redis/RedisReply.h"

#include "hiredis.h"

namespace PureDb {
struct PUREDB_API RedisConfig {
    std::string mIp;
    int mPort = 0;
    std::string mPass;
    uint16_t mDbIndex = 0;
    int32_t mConnectTimeout = 10000;
    int32_t mCommandTimeout = 10000;
    int32_t mKeepAliveInterval = 10000;

    PUREMSG_CLASS(mIp, mPort, mPass, mDbIndex, mConnectTimeout, mCommandTimeout, mKeepAliveInterval)
};

class PUREDB_API RedisConnector {
public:
    RedisConnector() = default;
    ~RedisConnector();

    bool is_alive() const;

    int connect(const RedisConfig& cfg);
    void close();

    void update();

    int pipe(PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params);
    template <typename... Params>
    int pipe_t(PureCore::StringRef cmd, Params&&... params) {
        int err = push_cmd(cmd);
        if (err != Success) {
            return err;
        }
        err = push_params(std::forward<Params>(params)...);
        if (err != Success) {
            return err;
        }
        return commit_cmd(true);
    }
    int pipe_format(const char* format, ...);

    int command(PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params);
    template <typename... Params>
    int command_t(PureCore::StringRef cmd, Params&&... params) {
        int err = push_cmd(cmd);
        if (err != Success) {
            return err;
        }
        err = push_params(std::forward<Params>(params)...);
        if (err != Success) {
            return err;
        }
        return commit_cmd(false);
    }
    int command_format(const char* format, ...);

    RedisReplyPtr pop_reply();

private:
    int push_cmd(PureCore::StringRef cmd);
    int push_param(PureCore::StringRef param);
    int push_params();
    template <typename Param1, typename... Params>
    int push_params(const Param1& param1, Params&&... params) {
        int err = push_param(param1);
        if (err != Success) {
            return err;
        }
        return push_params(std::forward<Params>(params)...);
    }
    int commit_cmd(bool isPipe);

private:
    void check_alive();
    int check_auth();
    int select_db();
    int reconnect();
    int get_reply();

    int real_pipe();
    int real_command();

    void clear_params();

private:
    redisContext* mContext = nullptr;
    RedisReplyPtr mReply;
    size_t mNeedReply = 0;
    int64_t mKeepAlive = 0;
    int32_t mKeepInterval = 10000;
    bool mAlive = false;
    std::string mPass;
    uint16_t mDbIndex = 0;

    std::vector<const char*> mParamsStr;
    std::vector<size_t> mParamsLen;
};

}  // namespace PureDb
