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
#include "PureCore/Buffer/BufferView.h"

namespace PureCore {
BufferView::BufferView() : mBuffer(), mReadPos(0), mWritePos(0) {}

BufferView::BufferView(const BufferView& cp) : mBuffer(cp.mBuffer), mReadPos(cp.mReadPos), mWritePos(cp.mWritePos) {}

BufferView::BufferView(DataRef data) : mBuffer(data), mReadPos(0), mWritePos(0) {}

BufferView::~BufferView() {
    mBuffer.reset(nullptr, 0);
    mReadPos = 0;
    mWritePos = 0;
}

BufferView& BufferView::operator=(const BufferView& right) {
    mBuffer = right.mBuffer;
    mReadPos = right.mReadPos;
    mWritePos = right.mWritePos;
    return *this;
}

void BufferView::clear() {
    mBuffer.reset(nullptr, 0);
    mReadPos = 0;
    mWritePos = 0;
}

void BufferView::reset(DataRef data) {
    mBuffer = data;
    mReadPos = 0;
    mWritePos = 0;
}

DataRef BufferView::data() const { return DataRef(mBuffer.data() + read_pos(), size()); }

DataRef BufferView::total_data() const { return DataRef(mBuffer.data(), total_size()); }

DataRef BufferView::buffer() const { return mBuffer; }

size_t BufferView::size() const { return write_pos() - read_pos(); }

size_t BufferView::total_size() const { return write_pos(); }

size_t BufferView::buffer_size() const { return mBuffer.size(); }

void BufferView::read_pos(size_t pos) {
    if (pos <= write_pos()) {
        mReadPos = pos;
    } else {
        mReadPos = write_pos();
    }
}

size_t BufferView::read_pos() const { return mReadPos; }

int BufferView::read(DataRef& data, size_t size) {
    int err = read_silent(data, size, read_pos());
    if (err != Success) {
        return err;
    }
    mReadPos += size;
    return Success;
}

int BufferView::read_silent(DataRef& data, size_t size, size_t offset) {
    if (write_pos() < offset + size) {  // out of range
        return ErrorDataOutOfRange;
    }

    data.reset(mBuffer.data() + offset, size);
    return Success;
}

void BufferView::write_pos(size_t pos) {
    if (pos <= buffer_size()) {
        mWritePos = pos;
    } else {
        mWritePos = buffer_size();
    }
}

size_t BufferView::write_pos() const { return mWritePos; }

int BufferView::write(DataRef data) {
    int err = write_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    mWritePos += data.size();
    return Success;
}

int BufferView::write_silent(DataRef data, size_t offset) {
    if (buffer_size() < offset + data.size()) {  // out of range
        return ErrorDataOutOfRange;
    }
    memcpy(mBuffer.data() + offset, data.data(), data.size());
    return Success;
}

int BufferView::write_char(char data) {
    int err = write_char_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    ++mWritePos;
    return Success;
}

int BufferView::write_char_silent(char data, size_t offset) {
    if (buffer_size() < offset + 1) {  // out of range
        return ErrorDataOutOfRange;
    }
    *(mBuffer.data() + offset) = data;
    return Success;
}

int BufferView::write_repeat_char(size_t count, char data) {
    int err = write_repeat_char_silent(count, data, write_pos());
    if (err != Success) {
        return err;
    }
    mWritePos += count;
    return Success;
}

int BufferView::write_repeat_char_silent(size_t count, char data, size_t offset) {
    if (buffer_size() < offset + count) {  // out of range
        return ErrorDataOutOfRange;
    }
    memset(mBuffer.data() + offset, data, count);
    return Success;
}

int BufferView::write_str(StringRef data) {
    int err = write_str_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    mWritePos += data.size();
    return Success;
}

int BufferView::write_str_silent(StringRef data, size_t offset) {
    if (buffer_size() < offset + data.size()) {  // out of range
        return ErrorDataOutOfRange;
    }
    memcpy(mBuffer.data() + offset, data.data(), data.size());
    return Success;
}

}  // namespace PureCore
