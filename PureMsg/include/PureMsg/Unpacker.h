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

#include "PureCore/Buffer/IBuffer.h"
#include "PureMsg/PureMsgLib.h"

#include <string>

namespace PureMsg {
PUREMSG_API uint8_t get_header(PureCore::IBuffer& buffer);
PUREMSG_API uint8_t read_header(PureCore::IBuffer& buffer);

PUREMSG_API int unpack_uint8(PureCore::IBuffer& buffer, uint8_t& d);
PUREMSG_API int unpack_uint16(PureCore::IBuffer& buffer, uint16_t& d);
PUREMSG_API int unpack_uint32(PureCore::IBuffer& buffer, uint32_t& d);
PUREMSG_API int unpack_uint64(PureCore::IBuffer& buffer, uint64_t& d);
PUREMSG_API int unpack_int8(PureCore::IBuffer& buffer, int8_t& d);
PUREMSG_API int unpack_int16(PureCore::IBuffer& buffer, int16_t& d);
PUREMSG_API int unpack_int32(PureCore::IBuffer& buffer, int32_t& d);
PUREMSG_API int unpack_int64(PureCore::IBuffer& buffer, int64_t& d);

PUREMSG_API int unpack_char(PureCore::IBuffer& buffer, char& d);
PUREMSG_API int unpack_signed_char(PureCore::IBuffer& buffer, signed char& d);
PUREMSG_API int unpack_short(PureCore::IBuffer& buffer, short& d);
PUREMSG_API int unpack_int(PureCore::IBuffer& buffer, int& d);
PUREMSG_API int unpack_long(PureCore::IBuffer& buffer, long& d);
PUREMSG_API int unpack_long_long(PureCore::IBuffer& buffer, long long& d);
PUREMSG_API int unpack_unsigned_char(PureCore::IBuffer& buffer, unsigned char& d);
PUREMSG_API int unpack_unsigned_short(PureCore::IBuffer& buffer, unsigned short& d);
PUREMSG_API int unpack_unsigned_int(PureCore::IBuffer& buffer, unsigned int& d);
PUREMSG_API int unpack_unsigned_long(PureCore::IBuffer& buffer, unsigned long& d);
PUREMSG_API int unpack_unsigned_long_long(PureCore::IBuffer& buffer, unsigned long long& d);

PUREMSG_API int unpack_float(PureCore::IBuffer& buffer, float& d);
PUREMSG_API int unpack_double(PureCore::IBuffer& buffer, double& d);

PUREMSG_API int unpack_nil(PureCore::IBuffer& buffer);
PUREMSG_API int unpack_skip(PureCore::IBuffer& buffer);
PUREMSG_API int unpack_bool(PureCore::IBuffer& buffer, bool& d);

PUREMSG_API int unpack_array(PureCore::IBuffer& buffer, uint32_t& l);

PUREMSG_API int unpack_map(PureCore::IBuffer& buffer, uint32_t& l);

PUREMSG_API int unpack_str(PureCore::IBuffer& buffer, uint32_t& l);

PUREMSG_API int unpack_string(PureCore::IBuffer& buffer, std::string& v);
#if PURE_CPP >= 201703L
PUREMSG_API int unpack_string_view(PureCore::IBuffer& buffer, std::string_view& v);
#endif
PUREMSG_API int unpack_string_ref(PureCore::IBuffer& buffer, PureCore::StringRef& v);

PUREMSG_API int unpack_bin(PureCore::IBuffer& buffer, uint32_t& l);

PUREMSG_API int unpack_data(PureCore::IBuffer& buffer, PureCore::IBuffer& v);
PUREMSG_API int unpack_data_ref(PureCore::IBuffer& buffer, PureCore::DataRef& v);

PUREMSG_API int unpack_obj(PureCore::IBuffer& buffer, uint32_t& count);

}  // namespace PureMsg
