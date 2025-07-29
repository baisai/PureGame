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

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/OsHelper.h"
#include "PureCore/SleepIdler.h"
#include "PureLua/LuaErrorDesc.h"
#include "PureNet/NetErrorDesc.h"
#include "PureEncrypt/PureZip.h"
#include "PureEncrypt/PureMD5.h"
#include "PureEncrypt/PureXxtea.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureApp/AppErrorDesc.h"
#include "PureApp/PureApp.h"
#include "PureApp/BindAllPureApp.h"
#include "PureApp/BindAllPureDb.h"
#include "PureApp/BindAllPureEncrypt.h"
#include "PureApp/BindAllPureMsg.h"
#include "PureApp/BindAllPureNet.h"

namespace PureApp {

const std::string& PureApp::name() const { return mName; }

void PureApp::set_hz(uint16_t hz) { mHz = hz; }

uint16_t PureApp::get_hz() const { return mHz; }

PureLua::PureLuaEnv& PureApp::lua() { return mLua; }

int PureApp::init(const std::string& name) {
    if (mRunning) {
        return ErrorInvalidState;
    }
    mTimeZero = PureCore::system_micro_s() - PureCore::steady_micro_s();
    mTimeOffset = 0;
    mName = name;
    int err = mTimer.init();
    if (err != PureCore::Success) {
        PureError("timer init failed {}", PureCore::get_error_desc(err));
        return ErrorAppInitFailed;
    }
    err = mLua.init();
    if (err != PureLua::Success) {
        PureError("lua init failed {}", PureLua::get_error_desc(err));
        return ErrorAppInitFailed;
    }

    bind_all_pure_db(mLua.state());
    bind_all_pure_encrypt(mLua.state());
    bind_all_pure_msg(mLua.state());
    bind_all_pure_net(mLua.state());
    bind_all_pure_app(mLua.state());
    mLua.set_global_var("timer__", &mTimer);
    mLua.set_global_var("net__", &mNet);
    mLua.set_global_var("app__", this);

    lua_getglobal(mLua.state(), "os");
    lua_pushlightuserdata(mLua.state(), (void*)this);
    lua_pushcclosure(
        mLua.state(),
        [](lua_State* L) -> int {
            PureApp* app = (PureApp*)lua_touserdata(L, lua_upvalueindex(1));
            lua_pushinteger(L, app->time_s());
            return 1;
        },
        1);
    lua_setfield(mLua.state(), -2, "time");
    lua_pop(mLua.state(), 1);

    mLua.add_require_loader([this](const char* fileName, PureCore::DynamicBuffer& buffer) {
        PureEncrypt::PureZipFileDecoder decoder;
        std::vector<std::string> files;
        auto ext = PureCore::get_file_ext(fileName);
        if (ext.empty()) {
            files.emplace_back(std::string(fileName) + ".lua");
            files.emplace_back(std::string(fileName) + ".luc");
        } else {
            files.emplace_back(fileName);
        }
        std::string pwd;
        for (auto& archive : this->mLuaArchive) {
            pwd = archive.second;
            int err = decoder.start_decode(archive.first.c_str());
            if (err != PureEncrypt::Success) {
                PureError("{} archive open failed", archive.first);
                continue;
            }

            err = PureEncrypt::ErrorFileNotExist;
            PureCore::StringRef fileName;
            for (auto& fn : files) {
                err = decoder.update_decode_entry(fn.c_str());
                if (err == PureEncrypt::Success) {
                    fileName = fn;
                    break;
                } else if (err != PureEncrypt::ErrorFileNotExist) {
                    PureError("{} archive decode {} failed", archive.first, fn);
                    return true;
                }
            }
            if (err != PureEncrypt::Success) {
                continue;
            }

            err = decoder.finish_decode();
            if (err != PureEncrypt::Success) {
                PureError("{} archive decode failed", archive.first);
                return true;
            }

            pwd.push_back(char(0xcd));
            pwd.append(fileName.data(), fileName.size());
            PureEncrypt::PureMD5 md;
            err = md.encode(pwd);
            if (err != PureEncrypt::Success) {
                PureError("{} archive calc md5 failed", archive.first);
                return true;
            }
            PureEncrypt::PureXxtea xxtea;
            err = xxtea.decode(decoder.get_output().data(), md.get_output().bytes());
            if (err != PureEncrypt::Success) {
                PureError("{} archive dencrypt {} failed", archive.first, fileName);
                return true;
            }
            xxtea.get_output().swap(buffer);
            return true;
        }

        return false;
    });

    mRunning = true;
    return Success;
}
void PureApp::stop() {
    if (!mRunning) {
        return;
    }
    mRunning = false;
}

bool PureApp::is_running() const { return mRunning; }

double PureApp::time() const { return double(mTimeZero + PureCore::steady_micro_s() + mTimeOffset) / double(1000 * 1000); }

bool PureApp::set_time(double s) {
    int64_t offset = int64_t((s - this->time()) * 1000.0 * 1000.0);
    if (offset <= 0) {
        return false;
    }
    mTimeOffset += offset;
    mIdler.set_offset(mTimeOffset / 1000);
    return true;
}

int64_t PureApp::time_s() const { return (mTimeZero + PureCore::steady_micro_s() + mTimeOffset) / (1000 * 1000); }

int64_t PureApp::time_milli_s() const { return (mTimeZero + PureCore::steady_micro_s() + mTimeOffset) / 1000; }

int64_t PureApp::time_micro_s() const { return (mTimeZero + PureCore::steady_micro_s() + mTimeOffset); }

int PureApp::add_lua_archive(const char* path, const char* pwd) {
    if (!PureCore::file_exist(path)) {
        return ErrorNotFoundFile;
    }
    if (pwd == nullptr) {
        pwd = "";
    }
    mLuaArchive.emplace_back(path, pwd);
    return Success;
}

void PureApp::clear_lua_archive() { mLuaArchive.clear(); }

void PureApp::loop() {
    mIdler.set_idle_delay(10 * 1000);
    mEventStart.notify();
    while (mRunning) {
        int64_t frameTime = 1000 / mHz;
        mIdler.frame_begin();
        mNet.update();
        int64_t delta = mIdler.frame_check(frameTime);
        if (delta > 0) {
            update(delta);
        }
        mIdler.frame_end(frameTime);
    }
    mEventEnd.notify();
    mTimer.release();
    mNet.stop();
    mLua.close();
}

void PureApp::update(int64_t delta) {
    mEventFrame.notify(delta);
    mTimer.update(delta);
}

}  // namespace PureApp
