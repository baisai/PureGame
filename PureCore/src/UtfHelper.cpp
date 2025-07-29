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

#include "PureCore/UtfHelper.h"

namespace PureCore {
size_t get_utf8_word_size(char ch) {
    if (0 <= uint8_t(ch) && uint8_t(ch) < 0x80) {
        return 1;
    }
    if (0xC2 <= uint8_t(ch) && uint8_t(ch) < 0xE0) {
        return 2;
    }
    if (0xE0 <= uint8_t(ch) && uint8_t(ch) < 0xF0) {
        return 3;
    }
    if (0xF0 <= uint8_t(ch) && uint8_t(ch) < 0xF8) {
        return 4;
    }
    return 0;
}

bool is_utf8_low(char ch) { return 0x80 <= uint8_t(ch) && uint8_t(ch) < 0xC0; }

bool is_utf16_high(char16_t ch) { return 0xD800 <= ch && ch < 0xDC00; }

bool is_utf16_low(char16_t ch) { return 0xDC00 <= ch && ch < 0xE000; }

size_t get_utf8_word_count(StringRef utf8) {
    size_t count = 0;
    for (auto iter = utf8.begin(); iter < utf8.end();) {
        size_t size = get_utf8_word_size((*iter));
        if (size == 0) {
            return count;
        }
        iter += size;
        ++count;
    }
    return count;
}

size_t get_utf16_word_count(String16Ref utf16) {
    size_t count = 0;
    for (auto iter = utf16.begin(); iter < utf16.end();) {
        if (is_utf16_high((*iter))) {
            iter += 2;
        } else {
            ++iter;
        }
        ++count;
    }
    return count;
}

int char_utf8_to_utf16(StringRef utf8, std::array<char16_t, 2>& utf16) {
    char32_t ch = 0;
    int err = char_utf8_to_utf32(utf8, ch);
    if (err != Success) {
        return err;
    }
    err = char_utf32_to_utf16(ch, utf16);
    if (err != Success) {
        return err;
    }
    return Success;
}

int char_utf8_to_utf32(StringRef utf8, char32_t& utf32) {
    if (utf8.empty()) {
        return ErrorInvalidUtf8;
    }
    size_t size = get_utf8_word_size(utf8[0]);
    if (size == 0) {
        return ErrorInvalidUtf8;
    }
    switch (size) {
        case 1:
            utf32 = char32_t(uint8_t(utf8[0]));
            break;
        case 2:
            if (utf8.size() < 2 || !is_utf8_low(utf8[1])) {
                return ErrorInvalidUtf8;
            }
            if ((uint8_t(utf8[0]) & 0x1E) == 0) {
                return ErrorInvalidUtf8;
            }
            utf32 = char32_t(utf8[0] & 0x1F) << 6;
            utf32 |= char32_t(utf8[1] & 0x3F);
            break;
        case 3:
            if (utf8.size() < 3 || !is_utf8_low(utf8[1]) || !is_utf8_low(utf8[2])) {
                return ErrorInvalidUtf8;
            }
            if ((uint8_t(utf8[0]) & 0x0F) == 0 && (uint8_t(utf8[1]) & 0x20) == 0) {
                return ErrorInvalidUtf8;
            }

            utf32 = char32_t(utf8[0] & 0x0F) << 12;
            utf32 |= char32_t(utf8[1] & 0x3F) << 6;
            utf32 |= char32_t(utf8[2] & 0x3F);
            break;
        case 4:
            if (utf8.size() < 4 || !is_utf8_low(utf8[1]) || !is_utf8_low(utf8[2]) || !is_utf8_low(utf8[3])) {
                return ErrorInvalidUtf8;
            }
            if ((uint8_t(utf8[0]) & 0x07) == 0 && (uint8_t(utf8[1]) & 0x30) == 0) {
                return ErrorInvalidUtf8;
            }

            utf32 = char32_t(utf8[0] & 0x07) << 18;
            utf32 |= char32_t(utf8[1] & 0x3F) << 12;
            utf32 |= char32_t(utf8[2] & 0x3F) << 6;
            utf32 |= char32_t(utf8[3] & 0x3F);
            break;
        default:
            return ErrorInvalidUtf8;
    }
    return Success;
}

int char_utf16_to_utf8(String16Ref utf16, std::array<char, 4>& utf8) {
    char32_t ch = 0;
    int err = char_utf16_to_utf32(utf16, ch);
    if (err != Success) {
        return err;
    }
    err = char_utf32_to_utf8(ch, utf8);
    if (err != Success) {
        return err;
    }
    return Success;
}

int char_utf16_to_utf32(String16Ref utf16, char32_t& utf32) {
    if (utf16.empty()) {
        return ErrorInvalidUtf16;
    }
    if (is_utf16_high(utf16[0])) {
        if (utf16.size() < 2) {
            return ErrorInvalidUtf16;
        }
        if (is_utf16_low(utf16[1])) {
            utf32 = 0x10000 + (char32_t(utf16[0]) - 0xD800) * 0x400 + (char32_t(utf16[1]) - 0xDC00);
        } else {
            return ErrorInvalidUtf16;
        }
    } else {
        utf32 = utf16[0];
    }
    return Success;
}

int char_utf32_to_utf8(const char32_t utf32, std::array<char, 4>& utf8) {
    if (utf32 > 0x10FFFF) {
        return ErrorInvalidUtf32;
    }

    if (utf32 < 0x80) {
        utf8[0] = char(utf32);
        utf8[1] = 0;
        utf8[2] = 0;
        utf8[3] = 0;
    } else if (utf32 < 0x800) {
        utf8[0] = 0xC0 | char(utf32 >> 6);
        utf8[1] = 0x80 | (char(utf32) & 0x3F);
        utf8[2] = 0;
        utf8[3] = 0;
    } else if (utf32 < 0x10000) {
        utf8[0] = 0xE0 | char(utf32 >> 12);
        utf8[1] = 0x80 | (char(utf32 >> 6) & 0x3F);
        utf8[2] = 0x80 | (char(utf32) & 0x3F);
        utf8[3] = 0;
    } else {
        utf8[0] = 0xF0 | char(utf32 >> 18);
        utf8[1] = 0x80 | (char(utf32 >> 12) & 0x3F);
        utf8[2] = 0x80 | (char(utf32 >> 6) & 0x3F);
        utf8[3] = 0x80 | (char(utf32) & 0x3F);
    }
    return Success;
}

int char_utf32_to_utf16(const char32_t utf32, std::array<char16_t, 2>& utf16) {
    if (utf32 > 0x10FFFF) {
        return ErrorInvalidUtf32;
    }

    if (utf32 < 0x10000) {
        utf16[0] = char16_t(utf32);
        utf16[1] = 0;
    } else {
        utf16[0] = char16_t((utf32 - 0x10000) / 0x400 + 0xD800);
        utf16[1] = char16_t((utf32 - 0x10000) % 0x400 + 0xDC00);
    }
    return Success;
}

int utf8_to_utf16(StringRef utf8, std::u16string& utf16) {
    for (auto iter = utf8.begin(); iter != utf8.end(); ++iter) {
        size_t size = get_utf8_word_size((*iter));
        if (size == 0) {
            return ErrorInvalidUtf8;
        }
        if (iter + size > utf8.end()) {
            return ErrorInvalidUtf8;
        }
        StringRef utf8Chars(iter, size);
        iter += (size - 1);
        std::array<char16_t, 2> utf16Chars{0, 0};
        int err = char_utf8_to_utf16(utf8Chars, utf16Chars);
        if (err != Success) {
            return err;
        }

        for (size_t i = 0; i < utf16Chars.size(); ++i) {
            if (utf16Chars[i] != 0) {
                utf16.push_back(utf16Chars[i]);
            }
        }
    }
    return Success;
}

int utf8_to_utf32(StringRef utf8, std::u32string& utf32) {
    for (auto iter = utf8.begin(); iter != utf8.end(); ++iter) {
        size_t size = get_utf8_word_size((*iter));
        if (size == 0) {
            return ErrorInvalidUtf8;
        }
        if (iter + size > utf8.end()) {
            return ErrorInvalidUtf8;
        }
        StringRef utf8Chars(iter, size);
        iter += (size - 1);
        char32_t utf32Char = 0;
        int err = char_utf8_to_utf32(utf8Chars, utf32Char);
        if (err != Success) {
            return err;
        }

        utf32.push_back(utf32Char);
    }
    return Success;
}

int utf16_to_utf8(String16Ref utf16, std::string& utf8) {
    for (auto iter = utf16.begin(); iter != utf16.end(); ++iter) {
        String16Ref utf16Chars;
        if (is_utf16_high((*iter))) {
            utf16Chars.reset(iter, 2);
            ++iter;
            if (iter == utf16.end()) {
                return ErrorInvalidUtf16;
            }
        } else {
            utf16Chars.reset(iter, 1);
        }

        std::array<char, 4> utf8Chars{0, 0, 0, 0};
        int err = char_utf16_to_utf8(utf16Chars, utf8Chars);
        if (err != Success) {
            return err;
        }
        for (size_t i = 0; i < utf8Chars.size(); ++i) {
            if (utf8Chars[i] != 0) {
                utf8.push_back(utf8Chars[i]);
            }
        }
    }
    return Success;
}

int utf16_to_utf32(String16Ref utf16, std::u32string& utf32) {
    for (auto iter = utf16.begin(); iter != utf16.end(); ++iter) {
        String16Ref utf16Chars;
        if (is_utf16_high((*iter))) {
            utf16Chars.reset(iter, 2);
            ++iter;
            if (iter == utf16.end()) {
                return ErrorInvalidUtf16;
            }
        } else {
            utf16Chars.reset(iter, 1);
        }

        char32_t utf32Char = 0;
        int err = char_utf16_to_utf32(utf16Chars, utf32Char);
        if (err != Success) {
            return err;
        }
        utf32.push_back(utf32Char);
    }
    return Success;
}

int utf32_to_utf8(String32Ref utf32, std::string& utf8) {
    for (auto iter = utf32.begin(); iter != utf32.end(); ++iter) {
        std::array<char, 4> utf8Chars{0, 0, 0, 0};
        int err = char_utf32_to_utf8(*iter, utf8Chars);
        if (err != Success) {
            return err;
        }

        for (size_t i = 0; i < utf8Chars.size(); ++i) {
            if (utf8Chars[i] != 0) {
                utf8.push_back(utf8Chars[i]);
            }
        }
    }
    return Success;
}

int utf32_to_utf16(String32Ref utf32, std::u16string& utf16) {
    for (auto iter = utf32.begin(); iter != utf32.end(); ++iter) {
        std::array<char16_t, 2> utf16Chars{0, 0};
        int err = char_utf32_to_utf16(*iter, utf16Chars);
        if (err != Success) {
            return err;
        }

        for (size_t i = 0; i < utf16Chars.size(); ++i) {
            if (utf16Chars[i] != 0) {
                utf16.push_back(utf16Chars[i]);
            }
        }
    }
    return Success;
}

int string_to_wstring(StringRef str, std::wstring& wstr) {
    if (sizeof(wchar_t) == sizeof(char16_t)) {
        std::u16string tmp;
        int err = utf8_to_utf16(str, tmp);
        if (err != Success) {
            return err;
        }
        wstr.append((const wchar_t*)tmp.c_str(), tmp.size());
        return Success;
    } else if (sizeof(wchar_t) == sizeof(char32_t)) {
        std::u32string tmp;
        int err = utf8_to_utf32(str, tmp);
        if (err != Success) {
            return err;
        }
        wstr.append((const wchar_t*)tmp.c_str(), tmp.size());
        return Success;
    }
    return ErrorInvalidWstring;
}

int wstring_to_string(WStringRef wstr, std::string& str) {
    if (sizeof(wchar_t) == sizeof(char16_t)) {
        std::u16string tmp;
        tmp.append((const char16_t*)wstr.data(), wstr.size());
        return utf16_to_utf8(tmp, str);
    } else if (sizeof(wchar_t) == sizeof(char32_t)) {
        std::u32string tmp;
        tmp.append((const char32_t*)wstr.data(), wstr.size());
        return utf32_to_utf8(tmp, str);
    }
    return ErrorInvalidWstring;
}
}  // namespace PureCore
