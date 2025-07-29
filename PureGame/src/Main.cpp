// Copyright 2023-present ChenDong, email <baisaichen@live.com>, All rights
// reserved.

#include "PureCore/PureLog.h"
#include "PureApp/PureExe.h"
#include "PureApp/PureApp.h"

#include <stdio.h>

int main(int argc, char** argv) {
    PureApp::PureExe exe;
    exe.init(argc, argv);
    PureApp::PureApp app;
    app.init(exe.get_name());
    app.lua().set_global_var("exe__", &exe);
    if (!exe.get_srcipt().empty()) {
        int err = app.lua().require_file(exe.get_srcipt().c_str());
        if (err != PureLua::Success) {
            return err;
        }
    }
    exe.signals().reg_signal(SIGINT, [&app](int num) { app.stop(); });
    app.loop();
    return 0;
}