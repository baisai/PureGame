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

#include "PureCore/DataRef.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureLua {
void bind_core_data_ref(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule lm(L, "PureCore");
    lm[PureLua::LuaRegisterClass<DataRef>(L, "DataRef")
           .default_ctor<StringRef>()
           .def([](DataRef& self, StringRef str) { self.reset(str.data(), str.size()); }, "reset")
           .def([](DataRef& self, size_t index) -> char { return self[index]; }, "get")
           .def(
               [](DataRef& self, size_t index, char c) -> bool {
                   if (index >= self.size()) {
                       return false;
                   }
                   self[index] = c;
                   return true;
               },
               "set")
           .def(&DataRef::bytes, "bytes")
           .def(&DataRef::empty, "empty")
           .def(&DataRef::size, "size")];
}
}  // namespace PureLua
