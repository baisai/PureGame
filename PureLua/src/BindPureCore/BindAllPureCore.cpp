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

#include "PureLua/BindAllPureCore.h"

namespace PureLua {

void bind_all_pure_core(lua_State* L) {
    bind_core_buffer(L);
    bind_core_error_desc(L);
    bind_core_version(L);
    bind_core_geometry(L);
    bind_core_incr_id_gen(L);
    bind_core_os_helper(L);
    bind_core_pure_json(L);
    bind_core_pure_xml(L);
    bind_core_quad_tree(L);
    bind_core_random_gen(L);
    bind_core_rb_timer(L);
    bind_core_rect_tree(L);
    bind_core_reuse_id_gen(L);
    bind_core_snow_id_gen(L);
    bind_core_time_counter(L);
    bind_core_tw_timer(L);
    bind_core_utf_helper(L);
    bind_core_data_ref(L);
}

}  // namespace PureLua
