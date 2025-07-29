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

#include "PureApp/BindAllPureEncrypt.h"

namespace PureApp {
void bind_all_pure_encrypt(lua_State* L) {
    bind_encrypt_error_desc(L);
    bind_encrypt_version(L);
    bind_encrypt_pure_base64(L);
    bind_encrypt_pure_crc32(L);
    bind_encrypt_pure_lz4(L);
    bind_encrypt_pure_md5(L);
    bind_encrypt_pure_sha1(L);
    bind_encrypt_pure_sha256(L);
    bind_encrypt_pure_sha512(L);
    bind_encrypt_pure_snappy(L);
    bind_encrypt_pure_xxtea(L);
    bind_encrypt_pure_zip(L);
}

}  // namespace PureApp
