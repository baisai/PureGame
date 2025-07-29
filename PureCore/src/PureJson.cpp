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

#include "PureCore/PureJson.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureCore/PureLog.h"

namespace PureCore {
static const yyjson_alc sJsonAlc{
    [](void *, size_t size) { return std::malloc(size); },
    [](void *, void *ptr, size_t, size_t size) { return std::realloc(ptr, size); },
    [](void *, void *ptr) { std::free(ptr); },
    nullptr,
};
///////////////////////////////////////////////////////////////////////////
// PureJsonItem
//////////////////////////////////////////////////////////////////////////
PureJsonItem::PureJsonItem(void *doc, yyjson_val *val) : mDoc(doc), mVal(val), mMutable(false) {}

PureJsonItem::PureJsonItem(void *doc, yyjson_mut_val *val) : mDoc(doc), mVal(val), mMutable(true) {}

bool PureJsonItem::is_null() const {
    if (mVal == nullptr) {
        return true;
    }
    return unsafe_yyjson_is_null(mVal);
}

PureJsonItem::operator bool() const { return !is_null(); }

bool PureJsonItem::is_bool() const {
    if (mVal == nullptr) {
        return false;
    }
    return unsafe_yyjson_is_bool(mVal);
}

bool PureJsonItem::is_int() const {
    if (mVal == nullptr) {
        return false;
    }
    return unsafe_yyjson_is_int(mVal);
}

bool PureJsonItem::is_float() const {
    if (mVal == nullptr) {
        return false;
    }
    return unsafe_yyjson_is_num(mVal);
}

bool PureJsonItem::is_str() const {
    if (mVal == nullptr) {
        return false;
    }
    return unsafe_yyjson_is_str(mVal);
}

bool PureJsonItem::is_obj() const {
    if (mVal == nullptr) {
        return false;
    }
    return unsafe_yyjson_is_obj(mVal);
}

bool PureJsonItem::is_arr() const {
    if (mVal == nullptr) {
        return false;
    }
    return unsafe_yyjson_is_arr(mVal);
}

bool PureJsonItem::is_obj_or_arr() const {
    if (mVal == nullptr) {
        return false;
    }
    return unsafe_yyjson_is_ctn(mVal);
}

bool PureJsonItem::get_as_bool() const {
    if (!is_bool()) {
        return false;
    }
    return unsafe_yyjson_get_bool(mVal);
}

int64_t PureJsonItem::get_as_int() const {
    if (!is_int()) {
        return 0;
    }
    return unsafe_yyjson_get_sint(mVal);
}

double PureJsonItem::get_as_float() const {
    if (!is_float()) {
        return 0.0;
    }
    return unsafe_yyjson_get_num(mVal);
}

StringRef PureJsonItem::get_as_str() const {
    if (!is_str()) {
        return StringRef();
    }
    const char *str = unsafe_yyjson_get_str(mVal);
    size_t len = unsafe_yyjson_get_len(mVal);
    return StringRef(str, len);
}

size_t PureJsonItem::size() const {
    if (is_str() || is_obj_or_arr()) {
        return unsafe_yyjson_get_len(mVal);
    }
    return 0;
}

PureJsonItem PureJsonItem::get_arr_child(size_t idx) const {
    if (!is_arr()) {
        return PureJsonItem();
    }
    if (mMutable) {
        return PureJsonItem(mDoc, yyjson_mut_arr_get((yyjson_mut_val *)mVal, idx));
    } else {
        return PureJsonItem(mDoc, yyjson_arr_get((yyjson_val *)mVal, idx));
    }
}

PureJsonItem PureJsonItem::get_child(StringRef name) const {
    if (!is_obj()) {
        return PureJsonItem();
    }
    if (mMutable) {
        return PureJsonItem(mDoc, yyjson_mut_obj_getn((yyjson_mut_val *)mVal, name.data(), name.size()));
    } else {
        return PureJsonItem(mDoc, yyjson_obj_getn((yyjson_val *)mVal, name.data(), name.size()));
    }
}

int PureJsonItem::set_null() {
    bool s = false;
    if (mMutable) {
        s = yyjson_mut_set_null((yyjson_mut_val *)mVal);
    } else {
        s = yyjson_set_null((yyjson_val *)mVal);
    }
    return s ? Success : ErrorNotSupport;
}

int PureJsonItem::set_bool(bool v) {
    bool s = false;
    if (mMutable) {
        s = yyjson_mut_set_bool((yyjson_mut_val *)mVal, v);
    } else {
        s = yyjson_set_bool((yyjson_val *)mVal, v);
    }
    return s ? Success : ErrorNotSupport;
}

int PureJsonItem::set_int(int64_t v) {
    bool s = false;
    if (mMutable) {
        s = yyjson_mut_set_sint((yyjson_mut_val *)mVal, v);
    } else {
        s = yyjson_set_sint((yyjson_val *)mVal, v);
    }
    return s ? Success : ErrorNotSupport;
}

int PureJsonItem::set_float(double v) {
    bool s = false;
    if (mMutable) {
        s = yyjson_mut_set_real((yyjson_mut_val *)mVal, v);
    } else {
        s = yyjson_set_real((yyjson_val *)mVal, v);
    }
    return s ? Success : ErrorNotSupport;
}

int PureJsonItem::set_str(StringRef v) {
    bool s = false;
    if (mMutable) {
        s = yyjson_mut_set_strn((yyjson_mut_val *)mVal, v.data(), v.size());
    } else {
        s = yyjson_set_strn((yyjson_val *)mVal, v.data(), v.size());
    }
    return s ? Success : ErrorNotSupport;
}

int PureJsonItem::append_arr_bool(bool v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_arr_add_bool((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, v)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureJsonItem::insert_arr_bool(size_t idx, bool v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    auto val = yyjson_mut_bool((yyjson_mut_doc *)mDoc, v);
    if (val == nullptr) {
        return ErrorInvalidArg;
    }
    if (!yyjson_mut_arr_insert((yyjson_mut_val *)mVal, val, idx)) {
        return ErrorInvalidArg;
    }
    return Success;
}

int PureJsonItem::append_arr_int(int64_t v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_arr_add_int((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, v)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureJsonItem::insert_arr_int(size_t idx, int64_t v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    auto val = yyjson_mut_int((yyjson_mut_doc *)mDoc, v);
    if (val == nullptr) {
        return ErrorInvalidArg;
    }
    if (!yyjson_mut_arr_insert((yyjson_mut_val *)mVal, val, idx)) {
        return ErrorInvalidArg;
    }
    return Success;
}

int PureJsonItem::append_arr_float(double v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_arr_add_real((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, v)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureJsonItem::insert_arr_float(size_t idx, double v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    auto val = yyjson_mut_real((yyjson_mut_doc *)mDoc, v);
    if (val == nullptr) {
        return ErrorInvalidArg;
    }
    if (!yyjson_mut_arr_insert((yyjson_mut_val *)mVal, val, idx)) {
        return ErrorInvalidArg;
    }
    return Success;
}

int PureJsonItem::append_arr_str(StringRef v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_arr_add_strn((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, v.data(), v.size())) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureJsonItem::insert_arr_str(size_t idx, StringRef v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    auto val = yyjson_mut_strn((yyjson_mut_doc *)mDoc, v.data(), v.size());
    if (val == nullptr) {
        return ErrorInvalidArg;
    }
    if (!yyjson_mut_arr_insert((yyjson_mut_val *)mVal, val, idx)) {
        return ErrorInvalidArg;
    }
    return Success;
}

PureJsonItem PureJsonItem::append_arr_arr() {
    if (!mMutable || mVal == nullptr) {
        return PureJsonItem();
    }
    auto arr = yyjson_mut_arr_add_arr((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal);
    if (arr == nullptr) {
        return PureJsonItem();
    }
    return PureJsonItem(mDoc, arr);
}

PureJsonItem PureJsonItem::insert_arr_arr(size_t idx) {
    if (!mMutable || mVal == nullptr) {
        return PureJsonItem();
    }
    auto val = yyjson_mut_arr((yyjson_mut_doc *)mDoc);
    if (val == nullptr) {
        return PureJsonItem();
    }
    if (!yyjson_mut_arr_insert((yyjson_mut_val *)mVal, val, idx)) {
        return PureJsonItem();
    }
    return PureJsonItem(mDoc, val);
}

PureJsonItem PureJsonItem::append_arr_obj() {
    if (!mMutable || mVal == nullptr) {
        return PureJsonItem();
    }
    auto obj = yyjson_mut_arr_add_obj((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal);
    if (obj == nullptr) {
        return PureJsonItem();
    }
    return PureJsonItem(mDoc, obj);
}

PureJsonItem PureJsonItem::insert_arr_obj(size_t idx) {
    if (!mMutable || mVal == nullptr) {
        return PureJsonItem();
    }
    auto val = yyjson_mut_obj((yyjson_mut_doc *)mDoc);
    if (val == nullptr) {
        return PureJsonItem();
    }
    if (!yyjson_mut_arr_insert((yyjson_mut_val *)mVal, val, idx)) {
        return PureJsonItem();
    }
    return PureJsonItem(mDoc, val);
}

int PureJsonItem::remove_arr_child(size_t idx) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    yyjson_mut_arr_remove((yyjson_mut_val *)mVal, idx);
    return Success;
}

int PureJsonItem::add_bool(const char *name, bool v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_obj_add_bool((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, name, v)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureJsonItem::add_int(const char *name, int64_t v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_obj_add_int((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, name, v)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureJsonItem::add_float(const char *name, double v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_obj_add_real((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, name, v)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureJsonItem::add_str(const char *name, StringRef v) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (!yyjson_mut_obj_add_strn((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, name, v.data(), v.size())) {
        return ErrorNotSupport;
    }
    return Success;
}

PureJsonItem PureJsonItem::add_arr(const char *name) {
    if (!mMutable || mVal == nullptr) {
        return PureJsonItem();
    }
    auto arr = yyjson_mut_arr((yyjson_mut_doc *)mDoc);
    if (arr == nullptr) {
        return PureJsonItem();
    }
    if (!yyjson_mut_obj_add_val((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, name, arr)) {
        return PureJsonItem();
    }
    return PureJsonItem(mDoc, arr);
}

PureJsonItem PureJsonItem::add_obj(const char *name) {
    if (!mMutable || mVal == nullptr) {
        return PureJsonItem();
    }
    auto obj = yyjson_mut_obj((yyjson_mut_doc *)mDoc);
    if (obj == nullptr) {
        return PureJsonItem();
    }
    if (!yyjson_mut_obj_add_val((yyjson_mut_doc *)mDoc, (yyjson_mut_val *)mVal, name, obj)) {
        return PureJsonItem();
    }
    return PureJsonItem(mDoc, obj);
}

int PureJsonItem::remove_child(StringRef name) {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    auto key = yyjson_mut_strn((yyjson_mut_doc *)mDoc, name.data(), name.size());
    if (key == nullptr) {
        return ErrorNotSupport;
    }
    yyjson_mut_obj_remove((yyjson_mut_val *)mVal, key);
    return Success;
}

int PureJsonItem::clear_children() {
    if (!mMutable || mVal == nullptr) {
        return ErrorNotSupport;
    }
    if (yyjson_mut_arr_clear((yyjson_mut_val *)mVal)) {
        return Success;
    }
    if (yyjson_mut_obj_clear((yyjson_mut_val *)mVal)) {
        return Success;
    }
    return ErrorNotSupport;
}

void PureJsonItem::range_arr(std::function<void(size_t idx, PureJsonItem val)> func) {
    if (!func) {
        return;
    }
    if (!is_arr()) {
        return;
    }
    if (mMutable) {
        yyjson_mut_arr_iter iter;
        auto val = (yyjson_mut_val *)mVal;
        yyjson_mut_arr_iter_init(val, &iter);
        size_t idx = 0;
        while (nullptr != (val = yyjson_mut_arr_iter_next(&iter))) {
            func(idx++, PureJsonItem(mDoc, val));
        }
    } else {
        yyjson_arr_iter iter;
        auto val = (yyjson_val *)mVal;
        yyjson_arr_iter_init(val, &iter);
        size_t idx = 0;
        while (nullptr != (val = yyjson_arr_iter_next(&iter))) {
            func(idx++, PureJsonItem(mDoc, val));
        }
    }
}

void PureJsonItem::range_obj(std::function<void(StringRef name, PureJsonItem val)> func) {
    if (!func) {
        return;
    }
    if (!is_arr()) {
        return;
    }
    if (mMutable) {
        yyjson_mut_val *key = nullptr, *val = nullptr;
        yyjson_mut_obj_iter iter;
        yyjson_mut_obj_iter_init((yyjson_mut_val *)mVal, &iter);
        while (nullptr != (key = yyjson_mut_obj_iter_next(&iter))) {
            val = yyjson_mut_obj_iter_get_val(key);
            PureCore::StringRef name(unsafe_yyjson_get_str(key), unsafe_yyjson_get_len(key));
            func(name, PureJsonItem(mDoc, val));
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// PureJson
//////////////////////////////////////////////////////////////////////////
PureJson::~PureJson() { clear(); }

int PureJson::load(DataRef data) {
    clear();
    yyjson_read_err err{};
    mDoc = yyjson_read_opts(data.data(), data.size(), 0, &sJsonAlc, &err);
    mMutable = false;
    if (err.code != YYJSON_READ_SUCCESS) {
        PureError(err.msg);
        return ErrorInvalidData;
    }
    return Success;
}

int PureJson::save(IBuffer &buffer, bool format) {
    if (mDoc == nullptr) {
        return ErrorInvalidData;
    }
    yyjson_write_flag flag = format ? YYJSON_WRITE_PRETTY : 0;
    const char *json = nullptr;
    size_t len = 0;
    yyjson_write_err jsonErr{};
    if (mMutable) {
        json = yyjson_mut_write_opts((yyjson_mut_doc *)mDoc, flag, &sJsonAlc, &len, &jsonErr);
    } else {
        json = yyjson_write_opts((yyjson_doc *)mDoc, flag, &sJsonAlc, &len, &jsonErr);
    }
    if (jsonErr.code != YYJSON_WRITE_SUCCESS) {
        if (json != nullptr) {
            sJsonAlc.free(nullptr, (void *)json);
        }
        PureError(jsonErr.msg);
        return ErrorNotSupport;
    }
    int err = buffer.write(DataRef(json, len));
    if (json != nullptr) {
        sJsonAlc.free(nullptr, (void *)json);
    }
    return err;
}

void PureJson::clear() {
    if (mDoc != nullptr) {
        if (mMutable) {
            yyjson_mut_doc_free((yyjson_mut_doc *)mDoc);
        } else {
            yyjson_doc_free((yyjson_doc *)mDoc);
        }
        mDoc = nullptr;
        mMutable = false;
    }
}

bool PureJson::has_root() {
    if (mDoc == nullptr) {
        return false;
    }
    if (mMutable) {
        return yyjson_mut_doc_get_root((yyjson_mut_doc *)mDoc) != nullptr;
    } else {
        return yyjson_doc_get_root((yyjson_doc *)mDoc) != nullptr;
    }
    return false;
}

PureJsonItem PureJson::get_root() {
    if (mDoc == nullptr) {
        return PureJsonItem();
    }
    if (mMutable) {
        return PureJsonItem(mDoc, yyjson_mut_doc_get_root((yyjson_mut_doc *)mDoc));
    } else {
        return PureJsonItem(mDoc, yyjson_doc_get_root((yyjson_doc *)mDoc));
    }
}

PureJsonItem PureJson::must_get_root() {
    auto item = get_root();
    if (item) {
        return item;
    }
    return create_root();
}

PureJsonItem PureJson::create_root(bool arr) {
    clear();
    auto doc = yyjson_mut_doc_new(nullptr);
    mDoc = doc;
    mMutable = true;
    if (mDoc == nullptr) {
        return PureJsonItem();
    }
    yyjson_mut_val *root = nullptr;
    if (arr) {
        root = yyjson_mut_arr(doc);
    } else {
        root = yyjson_mut_obj(doc);
    }
    yyjson_mut_doc_set_root(doc, root);
    return PureJsonItem(mDoc, root);
}

}  // namespace PureCore