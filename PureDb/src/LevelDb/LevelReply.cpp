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
#include "PureDb/Leveldb/LevelReply.h"

namespace PureDb {
enum ELevelReplyType : uint8_t {
    ETypeNone = 0,
    ETypeError = 1,
    ETypeBool = 2,
    ETypeInt = 4,
    ETypeKey = 8,
    ETypeValue = 16,
    ETypeArray = 32,
};
LevelReply::~LevelReply() { clear(); }

// thread safe
LevelReply* LevelReply::get() { return new LevelReply; }
// thread safe
void LevelReply::free(LevelReply* obj) {
    if (obj == nullptr) {
        return;
    }
    delete obj;
}

void LevelReply::clear() {
    mType = ETypeNone;
    mNum = 0;
    mKey.clear();
    mValue.clear();
    mArray.clear();
}

bool LevelReply::is_error() const { return mType == ETypeError; }

PureCore::StringRef LevelReply::get_error() const {
    if (is_error()) {
        return mValue;
    }
    return PureCore::StringRef();
}

void LevelReply::set_error(int err) {
    if (err != 0) {
        mType = ETypeError;
        mValue.assign(get_error_desc(err));
    }
}

bool LevelReply::has_int() const {
    if ((mType & ETypeInt) != 0) {
        return true;
    }
    return false;
}

int64_t LevelReply::get_int() const {
    if (has_int()) {
        return mNum;
    }
    return 0;
}

void LevelReply::set_int(int64_t num) {
    mType = ETypeInt;
    mNum = num;
}

bool LevelReply::has_bool() const {
    if ((mType & ETypeBool) != 0) {
        return true;
    }
    return false;
}

bool LevelReply::get_bool() const {
    if (has_bool()) {
        return mNum != 0;
    }
    return false;
}

void LevelReply::set_bool(bool v) {
    mType = ETypeBool;
    mNum = v ? 1 : 0;
}

bool LevelReply::has_key() const {
    if ((mType & ETypeKey) != 0) {
        return true;
    }
    return false;
}

PureCore::StringRef LevelReply::get_key() const {
    if (has_key()) {
        return mKey;
    }
    return PureCore::StringRef();
}

bool LevelReply::has_value() const {
    if ((mType & ETypeValue) != 0) {
        return true;
    }
    return false;
}

PureCore::StringRef LevelReply::get_value() const {
    if (has_value()) {
        return mValue;
    }
    return PureCore::StringRef();
}

bool LevelReply::is_array() const { return mType == ETypeArray; }

PureCore::ArrayRef<LevelReplyPtr> LevelReply::get_array() const {
    if (is_array()) {
        return mArray;
    }
    return PureCore::ArrayRef<LevelReplyPtr>();
}

LevelReply* LevelReply::get_array_item(size_t idx) const {
    if (is_array() && idx < mArray.size()) {
        return mArray[idx].get();
    }
    return nullptr;
}

size_t LevelReply::get_array_size() const {
    if (is_array()) {
        return mArray.size();
    }
    return 0;
}

void LevelReply::set_status(const leveldb::Status& sta) {
    if (!sta.ok()) {
        mType = ETypeError;
        mValue.assign(sta.ToString());
    }
}

void LevelReply::set_key(PureCore::StringRef key) {
    mType |= ETypeKey;
    mKey.assign(key.data(), key.size());
}

void LevelReply::clear_key() {
    mType &= (~ETypeKey);
    mKey.clear();
}

void LevelReply::set_value(PureCore::StringRef value) {
    mType |= ETypeValue;
    mValue.assign(value.data(), value.size());
}

std::string& LevelReply::use_value() {
    mType |= ETypeValue;
    mValue.clear();
    return mValue;
}

void LevelReply::clear_value() {
    mType &= (~ETypeValue);
    mValue.clear();
}

void LevelReply::capacity_array(size_t capacity) {
    mType = ETypeArray;
    mArray.reserve(capacity);
}

void LevelReply::append_array(LevelReplyPtr reply) {
    if (!reply) {
        return;
    }
    mType = ETypeArray;
    mArray.push_back(reply);
}
}  // namespace PureDb
