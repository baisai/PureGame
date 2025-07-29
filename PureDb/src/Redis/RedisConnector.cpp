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

#include "PureCore/PureLog.h"
#include "PureCore/OsHelper.h"
#include "PureDb/DbErrorDesc.h"
#include "PureDb/Redis/RedisConnector.h"

namespace PureDb {
RedisConnector::~RedisConnector() { close(); }

bool RedisConnector::is_alive() const { return mContext != nullptr && mAlive; }

int RedisConnector::connect(const RedisConfig& cfg) {
    if (mContext != nullptr) {
        return ErrorRedisStateInvalid;
    }
    redisOptions op{0};
    REDIS_OPTIONS_SET_TCP(&op, cfg.mIp.c_str(), cfg.mPort);
    if (PureCore::is_ip_v4(cfg.mIp.c_str())) {
        op.options |= REDIS_OPT_PREFER_IPV4;
    } else {
        op.options |= REDIS_OPT_PREFER_IPV6;
    }
    timeval tm1{cfg.mConnectTimeout / 1000, (cfg.mConnectTimeout % 1000) * 1000};
    timeval tm2{cfg.mCommandTimeout / 1000, (cfg.mCommandTimeout % 1000) * 1000};
    op.connect_timeout = &tm1;
    op.command_timeout = &tm2;
    mContext = redisConnectWithOptions(&op);
    if (mContext == nullptr) {
        return ErrorRedisConnectFailed;
    }
    if (mContext->err != REDIS_OK) {
        PureError("redis connect failed {}", mContext->errstr);
        redisFree(mContext);
        mContext = nullptr;
        return ErrorRedisConnectFailed;
    }
    mKeepInterval = cfg.mKeepAliveInterval;
    mPass = cfg.mPass;
    mDbIndex = cfg.mDbIndex;
    mKeepAlive = PureCore::steady_milli_s();
    mAlive = true;
    int err = check_auth();
    if (err != Success) {
        close();
        return err;
    }
    err = select_db();
    if (err != Success) {
        close();
        return err;
    }
    mAlive = true;
    return Success;
}

void RedisConnector::close() {
    if (mContext == nullptr) {
        return;
    }
    redisFree(mContext);
    mContext = nullptr;
    mReply.remove();
    mNeedReply = 0;
    clear_params();
}

void RedisConnector::update() {
    if (mContext == nullptr) {
        return;
    }
    int64_t now = PureCore::steady_milli_s();
    if (mAlive) {
        if (now - mKeepAlive >= mKeepInterval) {
            int err = command_t("ping");
            mKeepAlive = now;
            check_alive();
            if (err != Success) {
                PureError("redis ping failed {}", get_error_desc(err));
            }
            pop_reply();
        }
    } else {
        if (now - mKeepAlive >= 2000) {
            int err = reconnect();
            if (err != Success) {
                PureError("redis reconnect failed {}", get_error_desc(err));
                return;
            }
        }
    }
}

int RedisConnector::pipe(PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params) {
    int err = push_cmd(cmd);
    if (err != Success) {
        return err;
    }
    for (auto& param : params) {
        err = push_param(param);
        if (err != Success) {
            return err;
        }
    }
    return commit_cmd(true);
}

int RedisConnector::pipe_format(const char* format, ...) {
    if (format == nullptr) {
        return ErrorInvalidArg;
    }
    if (!is_alive()) {
        return ErrorRedisStateInvalid;
    }
    mKeepAlive = PureCore::steady_milli_s();
    va_list ap;
    va_start(ap, format);
    int err = redisvAppendCommand(mContext, format, ap);
    va_end(ap);
    check_alive();
    if (err != REDIS_OK) {
        return ErrorRedisCommandFailed;
    }
    return Success;
}

int RedisConnector::command(PureCore::StringRef cmd, PureCore::ArrayRef<PureCore::StringRef> params) {
    int err = push_cmd(cmd);
    if (err != Success) {
        return err;
    }
    for (auto& param : params) {
        err = push_param(param);
        if (err != Success) {
            return err;
        }
    }
    return commit_cmd(false);
}

int RedisConnector::command_format(const char* format, ...) {
    if (format == nullptr) {
        return ErrorInvalidArg;
    }
    if (!is_alive()) {
        return ErrorRedisStateInvalid;
    }
    mKeepAlive = PureCore::steady_milli_s();
    va_list ap;
    va_start(ap, format);
    void* r = redisvCommand(mContext, format, ap);
    va_end(ap);
    check_alive();
    redisReply* reply = (redisReply*)r;
    if (reply == nullptr) {
        return ErrorRedisCommandFailed;
    }
    mReply = RedisReply::get();
    if (!mReply) {
        freeReplyObject(r);
        return ErrorMemoryNotEnough;
    }
    int err = mReply->reset_from(reply);
    freeReplyObject(r);
    if (err != Success) {
        mReply.remove();
    }
    if (mReply && mReply->is_error()) {
        return ErrorRedisReplyError;
    }
    return err;
}

RedisReplyPtr RedisConnector::pop_reply() {
    if (!is_alive()) {
        return nullptr;
    }
    if (!mReply) {
        if (get_reply() != Success) {
            return nullptr;
        }
    }

    return mReply;
}

int RedisConnector::push_cmd(PureCore::StringRef cmd) {
    if (cmd.size() == 0) {
        return ErrorInvalidArg;
    }
    if (!is_alive()) {
        return ErrorRedisStateInvalid;
    }
    clear_params();
    mParamsStr.push_back(cmd.data());
    mParamsLen.push_back(cmd.size());
    return Success;
}

int RedisConnector::push_param(PureCore::StringRef param) {
    if (param.size() == 0) {
        return ErrorInvalidArg;
    }
    if (!is_alive()) {
        return ErrorRedisStateInvalid;
    }
    mParamsStr.push_back(param.data());
    mParamsLen.push_back(param.size());
    return Success;
}

int RedisConnector::push_params() {
    if (!is_alive()) {
        return ErrorRedisStateInvalid;
    }
    return Success;
}

int RedisConnector::commit_cmd(bool isPipe) {
    if (!is_alive()) {
        return ErrorRedisStateInvalid;
    }
    if (isPipe) {
        return real_pipe();
    }
    return real_command();
}

void RedisConnector::check_alive() {
    if (mContext == nullptr) {
        return;
    }
    if (mContext->err == REDIS_ERR_EOF || mContext->err == REDIS_ERR_IO || mContext->err == REDIS_ERR_OOM) {
        mAlive = false;
    }
    mContext->err = 0;
    mContext->errstr[0] = 0;
}

int RedisConnector::check_auth() {
    if (mPass.empty()) {
        return Success;
    }
    int err = command_format("auth %b", mPass.c_str(), mPass.size());
    if (err != Success) {
        return err;
    }
    auto reply = pop_reply();
    if (!reply || reply->is_error()) {
        err = ErrorRedisAuthFailed;
    }
    return err;
}

int RedisConnector::select_db() {
    int err = command_format("select %d", mDbIndex);
    if (err != Success) {
        return err;
    }
    auto reply = pop_reply();
    if (!reply || reply->is_error()) {
        err = ErrorRedisSelectDbFailed;
    }
    return err;
}

int RedisConnector::reconnect() {
    if (mContext == nullptr) {
        return ErrorRedisStateInvalid;
    }
    int err = redisReconnect(mContext);
    if (err != REDIS_OK) {
        return ErrorRedisConnectFailed;
    }
    mKeepAlive = PureCore::steady_milli_s();
    err = check_auth();
    if (err != Success) {
        return err;
    }
    err = select_db();
    if (err != Success) {
        return err;
    }
    mAlive = true;
    return Success;
}

int RedisConnector::get_reply() {
    if (mNeedReply <= 0) {
        return ErrorRedisGetReplyFailed;
    }
    if (!is_alive()) {
        return ErrorRedisStateInvalid;
    }
    redisReply* out = nullptr;
    int err = redisGetReply(mContext, (void**)&out);
    check_alive();
    if (err != REDIS_OK) {
        return ErrorRedisGetReplyError;
    }
    --mNeedReply;
    mReply = RedisReply::get();
    if (!mReply) {
        freeReplyObject(out);
        return ErrorMemoryNotEnough;
    }
    err = mReply->reset_from(out);
    if (err != Success) {
        mReply.remove();
    }
    freeReplyObject(out);
    return err;
}

int RedisConnector::real_pipe() {
    if (mParamsStr.empty() || mParamsStr.size() != mParamsLen.size()) {
        return ErrorInvalidArg;
    }
    mKeepAlive = PureCore::steady_milli_s();
    int err = redisAppendCommandArgv(mContext, int(mParamsStr.size()), &mParamsStr.front(), &mParamsLen.front());
    clear_params();
    check_alive();
    if (err != REDIS_OK) {
        return ErrorRedisCommandFailed;
    }
    ++mNeedReply;
    return Success;
}

int RedisConnector::real_command() {
    if (mParamsStr.empty() || mParamsStr.size() != mParamsLen.size()) {
        return ErrorInvalidArg;
    }
    mKeepAlive = PureCore::steady_milli_s();
    void* r = redisCommandArgv(mContext, int(mParamsStr.size()), &mParamsStr.front(), &mParamsLen.front());
    clear_params();
    check_alive();
    redisReply* reply = (redisReply*)r;
    if (reply == nullptr) {
        return ErrorRedisCommandFailed;
    }
    mReply = RedisReply::get();
    if (!mReply) {
        freeReplyObject(r);
        return ErrorMemoryNotEnough;
    }
    int err = mReply->reset_from(reply);
    freeReplyObject(r);
    if (err != Success) {
        mReply.remove();
    }
    if (mReply && mReply->is_error()) {
        return ErrorRedisReplyError;
    }
    return err;
}

void RedisConnector::clear_params() {
    mParamsStr.clear();
    mParamsLen.clear();
}

}  // namespace PureDb
