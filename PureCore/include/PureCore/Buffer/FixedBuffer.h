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
#include "PureCore/Buffer/BufferView.h"

namespace PureCore {
class PURECORE_API FixedBuffer : public IBuffer {
public:
    FixedBuffer(size_t size);
    FixedBuffer(const FixedBuffer& cp);
    FixedBuffer(FixedBuffer&& cp);
    virtual ~FixedBuffer();
    FixedBuffer& operator=(const FixedBuffer& right);
    FixedBuffer& operator=(FixedBuffer&& right);

    void swap(FixedBuffer& dest);

    virtual void clear();

    virtual DataRef data() const;
    virtual DataRef total_data() const;
    virtual DataRef buffer() const;
    virtual DataRef free_buffer() const;

    virtual size_t size() const;
    virtual size_t total_size() const;
    virtual size_t buffer_size() const;
    virtual size_t free_size() const;

    virtual BufferView view();

    virtual void read_pos(size_t pos);
    virtual size_t read_pos() const;
    virtual int read(DataRef& data, size_t size);
    virtual int read_silent(DataRef& data, size_t size, size_t offset = 0);

    virtual void write_pos(size_t pos);
    virtual size_t write_pos() const;
    virtual int write(DataRef data);
    virtual int write_silent(DataRef data, size_t offset = 0);
    virtual int write_char(char data);
    virtual int write_char_silent(char data, size_t offset = 0);
    virtual int write_repeat_char(size_t count, char data);
    virtual int write_repeat_char_silent(size_t count, char data, size_t offset = 0);
    virtual int write_str(StringRef data);
    virtual int write_str_silent(StringRef data, size_t offset = 0);

    virtual int resize_buffer(size_t size);

protected:
    BufferView mView;
    DataRef mBuffer;
};
}  // namespace PureCore
