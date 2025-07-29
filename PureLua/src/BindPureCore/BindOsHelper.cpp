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

#include "PureCore/OsHelper.h"

#include "PureLua/LuaModule.h"

namespace PureLua {
void bind_core_os_helper(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule(L, "PureCore")
        .def(steady_s, "steady_s")
        .def(steady_milli_s, "steady_milli_s")
        .def(steady_micro_s, "steady_micro_s")
        .def(system_s, "system_s")
        .def(system_milli_s, "system_milli_s")
        .def(system_micro_s, "system_micro_s")
        .def(remove_file, "remove_file")
        .def(rename_file, "rename_file")
        .def(file_exist, "file_exist")
        .def(dir_exist, "dir_exist")
        .def(get_parent_path, "get_parent_path")
        .def(get_path_leaf, "get_path_leaf")
        .def(get_file_ext, "get_file_ext")
        .def(get_trim_path, "get_trim_path")
        .def(create_dir, "create_dir")
        .def(is_absolute_path, "is_absolute_path")
        .def(file_size, "file_size")
        .def(read_file, "read_file")
        .def(write_file, "write_file")
        .def(
            [](lua_State* L) -> int {
                const char* path = PureLua::LuaStack<const char*>::get(L, 1);
                bool dir = PureLua::LuaStack<bool>::get(L, 2);
                bool recursion = PureLua::LuaStack<bool>::get(L, 3);
                std::vector<std::string> files;
                bool r = enum_dir_files(path, files, dir, recursion);
                if (!r) {
                    PureLuaErrorJump(L, "enum_dir_files failed");
                    return 0;
                }
                lua_createtable(L, int(files.size()), 0);
                for (int i = 0; i < files.size(); ++i) {
                    lua_pushlstring(L, files[i].c_str(), files[i].size());
                    lua_rawseti(L, -2, i + 1);
                }
                return 1;
            },
            "enum_dir_files")
        .def(errno_str, "errno_str")
        .def(get_used_virtual, "get_used_virtual")
        .def(get_total_virtual, "get_total_virtual")
        .def(get_used_physical, "get_used_physical")
        .def(get_total_physical, "get_total_physical")
        .def(get_bin_path, "get_bin_path")
        .def(PureCore::sleep, "sleep")
        .def(yeild, "yeild")
        .def(get_thread_id, "get_thread_id")
        .def(get_process_id, "get_process_id")
        .def(get_cpu_core, "get_cpu_core")
        .def(
            [](lua_State* L) -> int {
                StringRef four = PureLua::LuaStack<StringRef>::get(L, 1);
                if (four.size() != 4) {
                    PureLuaErrorJump(L, "arg is error, must 4 chars get {} chars", four.size());
                    return 0;
                }
                PureLua::LuaStack<uint32_t>::push(L, four_char_to_u32(four.data()));
                return 1;
            },
            "four_char_to_u32")
        .def(
            [](lua_State* L) -> int {
                uint32_t num = PureLua::LuaStack<uint32_t>::get(L, 1);
                char chars[4]{};
                u32_to_four_char(num, chars);
                lua_pushlstring(L, chars, 4);
                return 1;
            },
            "u32_to_four_char")
        .def(is_ip_v4, "is_ip_v4")
        .def(is_ip_v6, "is_ip_v6");
}
}  // namespace PureLua
