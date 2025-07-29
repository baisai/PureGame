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

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/StringRef.h"

#include <string>
#include <array>

namespace PureCore {
PURECORE_API size_t get_utf8_word_count(StringRef utf8);
PURECORE_API size_t get_utf16_word_count(String16Ref utf16);

PURECORE_API int char_utf8_to_utf16(StringRef utf8, std::array<char16_t, 2>& utf16);
PURECORE_API int char_utf8_to_utf32(StringRef utf8, char32_t& utf32);
PURECORE_API int char_utf16_to_utf8(String16Ref utf16, std::array<char, 4>& utf8);
PURECORE_API int char_utf16_to_utf32(String16Ref utf16, char32_t& utf32);
PURECORE_API int char_utf32_to_utf8(const char32_t utf32, std::array<char, 4>& utf8);
PURECORE_API int char_utf32_to_utf16(const char32_t utf32, std::array<char16_t, 2>& utf16);

PURECORE_API int utf8_to_utf16(StringRef utf8, std::u16string& utf16);
PURECORE_API int utf8_to_utf32(StringRef utf8, std::u32string& utf32);
PURECORE_API int utf16_to_utf8(String16Ref utf16, std::string& utf8);
PURECORE_API int utf16_to_utf32(String16Ref utf16, std::u32string& utf32);
PURECORE_API int utf32_to_utf8(String32Ref utf32, std::string& utf8);
PURECORE_API int utf32_to_utf16(String32Ref utf32, std::u16string& utf16);

PURECORE_API int string_to_wstring(StringRef str, std::wstring& wstr);
PURECORE_API int wstring_to_string(WStringRef wstr, std::string& str);
}  // namespace PureCore
