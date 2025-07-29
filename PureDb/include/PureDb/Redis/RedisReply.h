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
#include "PureCore/MovePtr.h"
#include "PureDb/PureDbLib.h"

struct redisReply;
namespace PureDb {
class RedisReply;
using RedisReplyPtr = PureCore::MovePtr<RedisReply, RedisReply>;
class PUREDB_API RedisReply {
public:
    friend class RedisConnector;
    RedisReply() = default;
    ~RedisReply();

    // thread safe
    static RedisReply* get();
    // thread safe
    static void free(RedisReply* obj);

    void clear();

    bool is_bool() const;
    bool get_bool() const;
    bool is_nil() const;
    bool get_nil() const;
    bool is_int() const;
    int64_t get_int() const;
    bool is_double() const;
    double get_double() const;
    bool is_error() const;
    PureCore::StringRef get_error() const;
    bool is_str() const;
    PureCore::StringRef get_str() const;
    bool is_array() const;
    PureCore::ArrayRef<RedisReplyPtr> get_array() const;
    RedisReply* get_array_item(size_t idx) const;

public:
    void set_bool(bool value);
    void set_nil();
    void set_int(int64_t value);
    void set_double(double value);
    int set_error(PureCore::StringRef str);
    int set_str(PureCore::StringRef str);
    void capacity_array(size_t capacity);
    void append_array(RedisReplyPtr reply);

private:
    int reset_from(redisReply* reply);
    PureCore::StringRef get_data() const;
    int set_data(PureCore::StringRef str);

private:
    uint8_t mType = 0;
    union {
        int64_t mInt;
        double mDouble;
    } mNumber{0};
    PureCore::DynamicBuffer mStr;
    std::vector<RedisReplyPtr> mArray;
};
}  // namespace PureDb
