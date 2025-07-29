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
#include "PureCore/MovePtr.h"
#include "PureDb/PureDbLib.h"

#include "leveldb/db.h"

namespace PureDb {
class LevelReply;
using LevelReplyPtr = PureCore::MovePtr<LevelReply, LevelReply>;
class PUREDB_API LevelReply {
public:
    LevelReply() = default;
    ~LevelReply();

    // thread safe
    static LevelReply* get();
    // thread safe
    static void free(LevelReply* obj);

    void clear();

    bool is_error() const;
    PureCore::StringRef get_error() const;
    void set_error(int err);

    bool has_int() const;
    int64_t get_int() const;
    void set_int(int64_t num);

    bool has_bool() const;
    bool get_bool() const;
    void set_bool(bool v);

    bool has_key() const;
    PureCore::StringRef get_key() const;

    bool has_value() const;
    PureCore::StringRef get_value() const;

    bool is_array() const;
    PureCore::ArrayRef<LevelReplyPtr> get_array() const;
    LevelReply* get_array_item(size_t idx) const;
    size_t get_array_size() const;

private:
    friend class LevelConnector;
    void set_status(const leveldb::Status& sta);
    void set_key(PureCore::StringRef key);
    void clear_key();
    void set_value(PureCore::StringRef value);
    std::string& use_value();
    void clear_value();
    void capacity_array(size_t capacity);
    void append_array(LevelReplyPtr reply);

private:
    uint8_t mType = 0;
    int64_t mNum = 0;
    std::string mKey;
    std::string mValue;
    std::vector<LevelReplyPtr> mArray;
};
}  // namespace PureDb
