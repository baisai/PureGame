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
#include "PureDb/DbErrorDesc.h"
#include "PureDb/Redis/RedisReply.h"

#include "hiredis.h"

namespace PureDb {
enum ERedisReplyType {
    ETypeNone = 0,
    ETypeError = 1,
    ETypeBool = 2,
    ETypeNil = 3,
    ETypeInt = 4,
    ETypeDouble = 5,
    ETypeStr = 6,
    ETypeArray = 7,
};
RedisReply::~RedisReply() { clear(); }

// thread safe
RedisReply* RedisReply::get() { return new RedisReply; }
// thread safe
void RedisReply::free(RedisReply* obj) {
    if (obj == nullptr) {
        return;
    }
    delete obj;
}

void RedisReply::clear() {
    mType = ETypeNone;
    mNumber = {0};
    mStr.clear();
    mArray.clear();
}

bool RedisReply::is_bool() const { return mType == ETypeBool; }

bool RedisReply::get_bool() const {
    if (is_bool()) {
        return mNumber.mInt != 0;
    }
    return false;
}

bool RedisReply::is_nil() const { return mType == ETypeNil; }

bool RedisReply::get_nil() const { return is_nil(); }

bool RedisReply::is_int() const { return mType == ETypeInt; }

int64_t RedisReply::get_int() const {
    if (is_int()) {
        return mNumber.mInt;
    }
    return 0;
}

bool RedisReply::is_double() const { return mType == ETypeDouble; }

double RedisReply::get_double() const {
    if (is_double()) {
        return mNumber.mDouble;
    }
    return 0.0;
}

bool RedisReply::is_error() const { return mType == ETypeError; }

PureCore::StringRef RedisReply::get_error() const {
    if (is_error()) {
        return get_data();
    }
    return PureCore::StringRef();
}

bool RedisReply::is_str() const { return mType == ETypeStr; }

PureCore::StringRef RedisReply::get_str() const {
    if (is_str()) {
        return get_data();
    }
    return PureCore::StringRef();
}

bool RedisReply::is_array() const { return mType == ETypeArray; }

PureCore::ArrayRef<RedisReplyPtr> RedisReply::get_array() const {
    if (is_array()) {
        return mArray;
    }
    return PureCore::ArrayRef<RedisReplyPtr>();
}

RedisReply* RedisReply::get_array_item(size_t idx) const {
    if (is_array() && idx < mArray.size()) {
        return mArray[idx].get();
    }
    return nullptr;
}

void RedisReply::set_bool(bool value) {
    mType = ETypeBool;
    mNumber.mInt = value ? 1 : 0;
}

void RedisReply::set_nil() { mType = ETypeNil; }

void RedisReply::set_int(int64_t value) {
    mType = ETypeInt;
    mNumber.mInt = value;
}

void RedisReply::set_double(double value) {
    mType = ETypeDouble;
    mNumber.mDouble = value;
}

int RedisReply::set_error(PureCore::StringRef str) {
    mType = ETypeError;
    return set_data(str);
}

int RedisReply::set_str(PureCore::StringRef str) {
    mType = ETypeStr;
    return set_data(str);
}

void RedisReply::capacity_array(size_t capacity) {
    mType = ETypeArray;
    mArray.reserve(capacity);
}

void RedisReply::append_array(RedisReplyPtr reply) {
    if (!reply) {
        return;
    }
    mType = ETypeArray;
    mArray.push_back(reply);
}

int RedisReply::reset_from(redisReply* reply) {
    if (reply == nullptr) {
        return ErrorNullPointer;
    }
    switch (reply->type) {
        case REDIS_REPLY_INTEGER:
            set_int(reply->integer);
            break;
        case REDIS_REPLY_NIL:
            set_nil();
            break;
        case REDIS_REPLY_BOOL:
            set_bool(reply->integer != 0);
            break;
        case REDIS_REPLY_DOUBLE:
            set_double(reply->dval);
            break;
        case REDIS_REPLY_ERROR:
            return set_error(PureCore::StringRef(reply->str, reply->len));
            break;
        case REDIS_REPLY_ARRAY:
        case REDIS_REPLY_MAP:
        case REDIS_REPLY_SET:
        case REDIS_REPLY_PUSH:
            capacity_array(reply->elements);
            for (size_t i = 0; i < reply->elements; ++i) {
                RedisReply* sub = RedisReply::get();
                if (sub == nullptr) {
                    return ErrorMemoryNotEnough;
                }
                append_array(sub);
                int err = sub->reset_from(reply->element[i]);
                if (err != Success) {
                    return err;
                }
            }
            break;
        case REDIS_REPLY_STATUS:
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_VERB:
        case REDIS_REPLY_BIGNUM:
            return set_str(PureCore::StringRef(reply->str, reply->len));
            break;
        default:
            return ErrorRedisGetReplyFailed;
            break;
    }
    return Success;
}

PureCore::StringRef RedisReply::get_data() const { return mStr.data().bytes(); }

int RedisReply::set_data(PureCore::StringRef str) {
    mStr.clear();
    int err = mStr.write(str);
    if (err != PureCore::Success) {
        return ErrorCoreBufferFailed;
    }
    return Success;
}
}  // namespace PureDb
