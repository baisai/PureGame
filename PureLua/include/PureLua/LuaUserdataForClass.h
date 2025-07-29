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

#include <functional>

namespace PureLua {
class LuaUserdataForClass {
public:
    template <typename T>
    LuaUserdataForClass(const T *p, int32_t offset, std::function<void(T *)> *gc, bool autoGC) : mClassOffset(0), mRaw((void *)p), mSelf(((char *)p) + offset) {
        if (gc && *gc && autoGC) {
            mAutoGC = autoGC;
            mLuaGC = [gc](void *p) { (*gc)((T *)p); };
        }
    }

    template <typename T>
    LuaUserdataForClass(const T *p, int32_t offset) : mClassOffset(0), mRaw((void *)p), mSelf(((char *)p) + offset) {}

    ~LuaUserdataForClass();

    void cast_object(int32_t offset);
    void set_class_offset(int32_t offset);
    void *get_object() const;
    void *move_object();

    template <typename T>
    inline T *get_object_t() {
        return (T *)get_object();
    }

    template <typename T>
    inline T *move_object_t() {
        return (T *)move_object();
    }

    void auto_gc();
    bool is_auto_gc() const;
    void set_auto_gc(bool autoGC);

private:
    mutable int32_t mClassOffset = 0;
    void *mRaw = nullptr;
    void *mSelf = nullptr;
    bool mAutoGC = false;
    std::function<void(void *)> mLuaGC{};
};

}  // namespace PureLua
