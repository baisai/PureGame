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
#include "PureLua/LuaErrorDesc.h"

namespace PureLua {
template <typename T>
class LuaResult : public T {
public:
    explicit LuaResult(const char* err) : mError(err) {}
    explicit LuaResult(const std::string& err) : mError(err) {}
    explicit LuaResult(std::string&& err) : mError(err) {}
    explicit LuaResult(PureCore::StringRef err) : mError(err.data(), err.size()) {}
#if PURE_CPP >= 201703L
    explicit LuaResult(std::string_view err) : mError(err.data(), err.size()) {}
#endif
    template <typename... Args>
    explicit LuaResult(Args&&... args) : T(std::forward<Args>(args)...) {}
    virtual ~LuaResult() {}

    inline PureCore::StringRef get_error_desc() const { return mError; }
    inline bool is_success() const { return mError.empty(); }

private:
    std::string mError;
};

}  // namespace PureLua
