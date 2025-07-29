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

#include "PureApp/AppErrorDesc.h"
#include "PureApp/PureExe.h"
#include "PureApp/PureApp.h"
#include "PureApp/PureMain.h"

namespace PureApp {
int pure_main(int argc, char** argv) {
    PureExe exe;
    exe.init(argc, argv);
    if (exe.get_srcipt().empty()) {
        PureError("not found script");
        return -1;
    }
    PureApp app;
    app.init(exe.get_name());
    app.lua().set_global_var("exe__", &exe);
    app.lua().add_package_path(exe.bin_dir() + "?.lua");
    int err = app.lua().require_file(exe.get_srcipt().c_str());
    if (err != PureLua::Success) {
        return err;
    }
    exe.signals().reg_signal(SIGINT, [&app](int num) { app.stop(); });
    app.loop();
    return 0;
}
}  // namespace PureApp
