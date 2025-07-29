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

#include "PureLua/LuaUserdataForClass.h"

namespace PureLua {
LuaUserdataForClass::~LuaUserdataForClass() { auto_gc(); }

void LuaUserdataForClass::cast_object(int32_t offset) { mSelf = static_cast<char *>(mSelf) + offset; }

void LuaUserdataForClass::set_class_offset(int32_t offset) { mClassOffset = offset; }

void *LuaUserdataForClass::get_object() const {
    char *p = (char *)mSelf + mClassOffset;
    mClassOffset = 0;
    return p;
}

void *LuaUserdataForClass::move_object() {
    char *p = (char *)mSelf + mClassOffset;
    mClassOffset = 0;
    mRaw = nullptr;
    mSelf = nullptr;
    return p;
}

void LuaUserdataForClass::auto_gc() {
    if (mRaw && mAutoGC && mLuaGC) {
        mLuaGC(mRaw);
    }
    mRaw = nullptr;
    mSelf = nullptr;
}

bool LuaUserdataForClass::is_auto_gc() const { return mAutoGC; }

void LuaUserdataForClass::set_auto_gc(bool autoGC) { mAutoGC = autoGC; }

}  // namespace PureLua
