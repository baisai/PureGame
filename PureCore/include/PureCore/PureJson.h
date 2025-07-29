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

#include "PureCore/PureCoreLib.h"
#include "PureCore/Buffer/IBuffer.h"

#include "yyjson.h"

#include <functional>

namespace PureCore {
class PURECORE_API PureJsonItem {
public:
    PureJsonItem() = default;
    PureJsonItem(void* doc, yyjson_val* val);
    PureJsonItem(void* doc, yyjson_mut_val* val);

    bool is_null() const;
    explicit operator bool() const;
    bool is_bool() const;
    bool is_int() const;
    bool is_float() const;
    bool is_str() const;
    bool is_obj() const;
    bool is_arr() const;
    bool is_obj_or_arr() const;

    bool get_as_bool() const;
    int64_t get_as_int() const;
    double get_as_float() const;
    StringRef get_as_str() const;
    size_t size() const;
    PureJsonItem get_arr_child(size_t idx) const;
    PureJsonItem get_child(StringRef name) const;

    int set_null();
    int set_bool(bool v);
    int set_int(int64_t v);
    int set_float(double v);
    int set_str(StringRef v);

    int append_arr_bool(bool v);
    int insert_arr_bool(size_t idx, bool v);
    int append_arr_int(int64_t v);
    int insert_arr_int(size_t idx, int64_t v);
    int append_arr_float(double v);
    int insert_arr_float(size_t idx, double v);
    int append_arr_str(StringRef v);
    int insert_arr_str(size_t idx, StringRef v);
    PureJsonItem append_arr_arr();
    PureJsonItem insert_arr_arr(size_t idx);
    PureJsonItem append_arr_obj();
    PureJsonItem insert_arr_obj(size_t idx);
    int remove_arr_child(size_t idx);

    int add_bool(const char* name, bool v);
    int add_int(const char* name, int64_t v);
    int add_float(const char* name, double v);
    int add_str(const char* name, StringRef v);
    PureJsonItem add_arr(const char* name);
    PureJsonItem add_obj(const char* name);
    int remove_child(StringRef name);
    int clear_children();

    void range_arr(std::function<void(size_t idx, PureJsonItem val)> func);
    void range_obj(std::function<void(StringRef name, PureJsonItem val)> func);

private:
    void* mDoc = nullptr;
    void* mVal = nullptr;
    bool mMutable = false;
};

class PURECORE_API PureJson {
public:
    PureJson() = default;
    ~PureJson();

    int load(DataRef data);
    int save(IBuffer& buffer, bool format = true);
    void clear();

    bool has_root();
    PureJsonItem get_root();
    PureJsonItem must_get_root();
    PureJsonItem create_root(bool arr = false);

private:
    void* mDoc = nullptr;
    bool mMutable = false;

    PURE_DISABLE_COPY(PureJson)
};
}  // namespace PureCore
