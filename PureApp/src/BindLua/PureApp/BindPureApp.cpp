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

#include "PureApp/PureApp.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_app_pure_app(lua_State* L) {
    PureLua::LuaModule lm(L, "PureApp");
    lm[PureLua::LuaRegisterClass<PureApp>(L, "PureApp")
           .def(&PureApp::name, "name")
           .def(&PureApp::set_hz, "set_hz")
           .def(&PureApp::get_hz, "get_hz")
           .def(&PureApp::stop, "stop")
           .def(&PureApp::is_running, "is_running")
           .def(&PureApp::time, "time")
           .def(&PureApp::set_time, "set_time")
           .def(&PureApp::time_s, "time_s")
           .def(&PureApp::time_milli_s, "time_milli_s")
           .def(&PureApp::time_micro_s, "time_micro_s")
           .def(&PureApp::add_lua_archive, "add_lua_archive")
           .def(&PureApp::clear_lua_archive, "clear_lua_archive")
           .def([](PureApp& self, std::function<bool()> cb) { return self.mEventStart.bind(cb); }, "listen_event_start")
           .def([](PureApp& self, int64_t id) { return self.mEventStart.unbind(id); }, "stop_event_start")
           .def([](PureApp& self, std::function<bool(int64_t)> cb) { return self.mEventFrame.bind(cb); }, "listen_event_frame")
           .def([](PureApp& self, int64_t id) { return self.mEventFrame.unbind(id); }, "stop_event_frame")
           .def([](PureApp& self, std::function<bool()> cb) { return self.mEventEnd.bind(cb); }, "listen_event_end")
           .def([](PureApp& self, int64_t id) { return self.mEventEnd.unbind(id); }, "stop_event_end")];
}
}  // namespace PureApp