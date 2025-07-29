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

#include "PureCore/ArgParser.h"
#include "PureCore/OsHelper.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureEncrypt/PureMD5.h"
#include "PureEncrypt/PureZip.h"
#include "PureEncrypt/PureXxtea.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lopnames.h"
#include "lstate.h"
#include "lundump.h"
}

#include <iostream>

static void Usage() {
    std::cout << "Usage: PureLuaArchive -s srcDir -d destFile -k key -c" << std::endl;
    getchar();
}

static int encrypt(PureCore::DynamicBuffer& buffer, PureCore::StringRef key) {
    if (key.empty()) {
        return PureEncrypt::ErrorDataInvalid;
    }
    PureEncrypt::PureMD5 md;
    int err = md.encode(key);
    if (err != PureEncrypt::Success) {
        return err;
    }
    PureEncrypt::PureXxtea encoder;
    err = encoder.encode(buffer.data(), md.get_output().bytes());
    if (err != PureEncrypt::Success) {
        return err;
    }
    encoder.get_output().swap(buffer);
    return PureEncrypt::Success;
}
static int encode_lua(PureEncrypt::PureZipFileEncoder& encoder, const char* path, PureCore::StringRef key) {
    std::string filePath = PureCore::get_trim_path(path);
    if (filePath.back() != '/') {
        filePath.push_back('/');
    }
    size_t startIdx = filePath.size();
    std::vector<std::string> files;
    PureCore::DynamicBuffer buffer;
    bool succ = PureCore::enum_dir_files(path, files, false, true);
    if (!succ) {
        return PureEncrypt::ErrorPathNotExist;
    }
    std::string keyStr(key.data(), key.size());
    keyStr.push_back(char(0xcd));
    size_t keyIdx = keyStr.size();
    for (auto& fn : files) {
        buffer.clear();
        filePath.resize(startIdx);
        filePath.append(fn);
        int err = PureCore::read_file(buffer, filePath.c_str());
        if (err != PureCore::Success) {
            return PureEncrypt::ErrorFileNotExist;
        }
        keyStr.resize(keyIdx);
        keyStr.append(fn);
        err = encrypt(buffer, keyStr);
        if (err != PureEncrypt::Success) {
            return err;
        }
        err = encoder.update_encode_entry(fn.c_str(), buffer.data());
        if (err != PureEncrypt::Success) {
            return err;
        }
    }
    return PureEncrypt::Success;
}

#define toproto(L, i) getproto(s2v(L->top.p + (i)))
#define FUNCTION "(function()end)();\n"
static const Proto* combine(lua_State* L, int n) {
    if (n == 1) {
        return toproto(L, -1);
    } else {
        Proto* f = nullptr;
        int i = n;
        if (lua_load(
                L,
                [](lua_State* L, void* ud, size_t* size) -> const char* {
                    UNUSED(L);
                    if ((*(int*)ud)--) {
                        *size = sizeof(FUNCTION) - 1;
                        return FUNCTION;
                    } else {
                        *size = 0;
                        return NULL;
                    }
                },
                &i, "=(luac)", NULL) != LUA_OK) {
            luaL_error(L, lua_tostring(L, -1));
            return f;
        }
        f = toproto(L, -1);
        for (i = 0; i < n; i++) {
            f->p[i] = toproto(L, i - n - 1);
            if (f->p[i]->sizeupvalues > 0) f->p[i]->upvalues[0].instack = 0;
        }
        return f;
    }
}
static int encode_luac(PureEncrypt::PureZipFileEncoder& encoder, const char* path, PureCore::StringRef key) {
    lua_State* L = luaL_newstate();
    if (L == nullptr) {
        return PureEncrypt::ErrorNullPointer;
    }
    lua_pushcclosure(
        L,
        [](lua_State* L) -> int {
            PureEncrypt::PureZipFileEncoder* encoder = (PureEncrypt::PureZipFileEncoder*)lua_touserdata(L, 1);
            const char* path = lua_tostring(L, 2);
            size_t len = 0;
            const char* str = lua_tolstring(L, 3, &len);
            PureCore::StringRef key(str, len);
            std::string filePath = PureCore::get_trim_path(path);
            if (filePath.back() != '/') {
                filePath.push_back('/');
            }
            size_t startIdx = filePath.size();
            std::vector<std::string> files;
            PureCore::DynamicBuffer buffer;
            bool succ = PureCore::enum_dir_files(path, files, false, true);
            if (!succ) {
                luaL_error(L, "enum dir `%s` files failed", path);
                return 0;
            }
            std::string keyStr(key.data(), key.size());
            keyStr.push_back(char(0xcd));
            size_t keyIdx = keyStr.size();
            for (auto& fn : files) {
                filePath.resize(startIdx);
                filePath.append(fn);
                int top = lua_gettop(L);
                if (luaL_loadfile(L, filePath.c_str()) != LUA_OK) {
                    luaL_error(L, lua_tostring(L, -1));
                    return 0;
                }
                auto f = combine(L, 1);
                if (f == nullptr) {
                    luaL_error(L, "lua combine failed");
                    return 0;
                }
                buffer.clear();
                lua_lock(L);
                luaU_dump(
                    L, f,
                    [](lua_State* L, const void* p, size_t size, void* u) -> int {
                        PureCore::DynamicBuffer* buffer = (PureCore::DynamicBuffer*)u;
                        int writeErr = buffer->write(PureCore::DataRef(p, size));
                        if (writeErr != 0) {
                            luaL_error(L, "buffer write failed");
                            return 1;
                        }
                        return 0;
                    },
                    (void*)&buffer, 0);
                lua_unlock(L);
                lua_settop(L, top);

                keyStr.resize(keyIdx);
                keyStr.append(fn);
                int err = encrypt(buffer, keyStr);
                if (err != PureEncrypt::Success) {
                    luaL_error(L, PureEncrypt::get_error_desc(err).c_str());
                    return 0;
                }
                err = encoder->update_encode_entry(fn.c_str(), buffer.data());
                if (err != PureEncrypt::Success) {
                    luaL_error(L, PureEncrypt::get_error_desc(err).c_str());
                    return 0;
                }
            }
            return 0;
        },
        0);
    lua_pushlightuserdata(L, (void*)&encoder);
    lua_pushstring(L, path);
    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
        const char* luaStr = lua_tostring(L, -1);
        std::cerr << luaStr << std::endl;
        return PureEncrypt::ErrorZipEncodeFailed;
    }
    lua_close(L);
    return PureEncrypt::Success;
}

int main(int argc, char** argv) {
    PureCore::ArgParser parser;
    parser.parser(argc, argv);
    if (parser.size() < 2) {
        Usage();
        return -1;
    }

    PureCore::StringRef src, dest, key;
    bool luac = false;
    for (size_t i = 0; i < parser.size(); ++i) {
        if (parser[i].mOption == "-s") {
            src = parser[i].mValue;
        } else if (parser[i].mOption == "-d") {
            dest = parser[i].mValue;
        } else if (parser[i].mOption == "-k") {
            key = parser[i].mValue;
        } else if (parser[i].mOption == "-c") {
            luac = true;
        }
    }
    if (src.empty() || dest.empty()) {
        Usage();
        return -2;
    }

    if (!PureCore::dir_exist(src.data())) {
        std::cerr << "src dir is not exist" << std::endl;
        return -3;
    }

    PureEncrypt::PureZipFileEncoder encoder;
    int err = encoder.start_encode(dest.data(), false);
    if (err != PureEncrypt::Success) {
        std::cerr << PureEncrypt::get_error_desc(err) << std::endl;
        return -4;
    }
    if (luac) {
        err = encode_luac(encoder, src.data(), key);
    } else {
        err = encode_lua(encoder, src.data(), key);
    }
    if (err != PureEncrypt::Success) {
        std::cerr << PureEncrypt::get_error_desc(err) << std::endl;
        return -5;
    }
    err = encoder.finish_encode();
    if (err != PureEncrypt::Success) {
        std::cerr << PureEncrypt::get_error_desc(err) << std::endl;
        return -6;
    }
    return 0;
}
