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

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/PureLog.h"
#include "PureCore/OsHelper.h"
#include "PureDb/DbErrorDesc.h"
#include "PureDb/Redis/RedisAsyncPool.h"

namespace PureDb {
RedisAsyncPool::~RedisAsyncPool() { clear(); }

int RedisAsyncPool::init(int32_t count, int64_t reqTimeout, const RedisConfig& cfg) {
    if (count <= 0) {
        count = PureCore::get_cpu_core();
    }
    clear();
    for (int32_t i = 0; i < count; ++i) {
        int err = add_connector(reqTimeout, cfg);
        if (err != Success) {
            clear();
            PureError("RedisAsyncPool init {}:{} failed", cfg.mIp, cfg.mPort);
            return err;
        }
    }
    return Success;
}

void RedisAsyncPool::clear() {
    for (const auto con : mPool) {
        con->stop();
        con->join(0);
        delete con;
    }
    mPool.clear();
}

void RedisAsyncPool::update() {
    for (const auto con : mPool) {
        con->update();
    }
}

RedisAsyncConnector* RedisAsyncPool::get_connector() {
    if (mPool.empty()) {
        return nullptr;
    }
    for (const auto con : mPool) {
        if (con->is_busying()) {
            continue;
        }
        return con;
    }

    auto idx = mRandom.gen_less_int(uint32_t(mPool.size()));
    return mPool[idx];
}

int RedisAsyncPool::add_connector(int64_t reqTimeout, const RedisConfig& cfg) {
    auto con = new RedisAsyncConnector;
    if (con == nullptr) {
        return ErrorMemoryNotEnough;
    }
    int err = con->init(reqTimeout);
    if (err != Success) {
        delete con;
        return err;
    }
    err = con->run();
    if (err != PureCore::Success) {
        con->stop();
        con->join(0);
        delete con;
        return ErrorRedisConnectFailed;
    }
    err = con->connect(
        [](RedisReplyPtr reply) {
            if (reply->is_error()) {
                PureError("RedisAsyncPool connect error `{}`", reply->get_error());
            } else {
                PureError("RedisAsyncPool connect success");
            }
        },
        cfg);
    if (err != Success) {
        con->stop();
        con->join(0);
        delete con;
        return err;
    }
    mPool.push_back(con);
    return Success;
}

}  // namespace PureDb
