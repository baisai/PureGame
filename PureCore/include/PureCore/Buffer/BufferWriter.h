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

#include "PureCore/PureCoreLib.h"
#include "PureCore/DataRef.h"

namespace PureCore {
class PURECORE_API BufferWriter {
public:
    BufferWriter();
    BufferWriter(const BufferWriter& cp);
    BufferWriter(DataRef data);
    ~BufferWriter();

    BufferWriter& operator=(const BufferWriter& right);

    void clear();
    void reset(DataRef data);

    DataRef data() const;
    DataRef total_data() const;
    DataRef buffer() const;

    size_t size() const;
    size_t total_size() const;
    size_t buffer_size() const;

    void write_pos(size_t pos);
    size_t write_pos() const;
    int write(DataRef data);
    int write_silent(DataRef data, size_t offset = 0);
    int write_char(char data);
    int write_char_silent(char data, size_t offset = 0);
    int write_repeat_char(size_t count, char data);
    int write_repeat_char_silent(size_t count, char data, size_t offset = 0);
    int write_str(StringRef data);
    int write_str_silent(StringRef data, size_t offset = 0);

protected:
    DataRef mBuffer;
    size_t mWritePos;
};

}  // namespace PureCore
