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
#include "PureCore/Buffer/BufferView.h"

namespace PureCore {
class PURECORE_API IBuffer {
public:
    virtual ~IBuffer() {}

    virtual void clear() = 0;

    virtual DataRef data() const = 0;
    virtual DataRef total_data() const = 0;
    virtual DataRef buffer() const = 0;
    virtual DataRef free_buffer() const = 0;

    virtual size_t size() const = 0;
    virtual size_t total_size() const = 0;
    virtual size_t buffer_size() const = 0;
    virtual size_t free_size() const = 0;

    virtual BufferView view() = 0;

    virtual void read_pos(size_t pos) = 0;
    virtual size_t read_pos() const = 0;
    virtual int read(DataRef& data, size_t size) = 0;
    virtual int read_silent(DataRef& data, size_t size, size_t offset = 0) = 0;

    virtual void write_pos(size_t pos) = 0;
    virtual size_t write_pos() const = 0;
    virtual int write(DataRef data) = 0;
    virtual int write_silent(DataRef data, size_t offset = 0) = 0;
    virtual int write_char(char data) = 0;
    virtual int write_char_silent(char data, size_t offset = 0) = 0;
    virtual int write_repeat_char(size_t count, char data) = 0;
    virtual int write_repeat_char_silent(size_t count, char data, size_t offset = 0) = 0;
    virtual int write_str(StringRef data) = 0;
    virtual int write_str_silent(StringRef data, size_t offset = 0) = 0;

    virtual int resize_buffer(size_t size) = 0;
};
}  // namespace PureCore
