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
#include "PureCore/Buffer/BufferWriter.h"

namespace PureCore {
BufferWriter::BufferWriter() : mBuffer(), mWritePos(0) {}

BufferWriter::BufferWriter(const BufferWriter& cp) : mBuffer(cp.mBuffer), mWritePos(cp.mWritePos) {}

BufferWriter::BufferWriter(DataRef data) : mBuffer(data), mWritePos(0) {}

BufferWriter::~BufferWriter() {
    mBuffer.reset(nullptr, 0);
    mWritePos = 0;
}

BufferWriter& BufferWriter::operator=(const BufferWriter& right) {
    mBuffer = right.mBuffer;
    mWritePos = right.mWritePos;
    return *this;
}

void BufferWriter::clear() {
    mBuffer.reset(nullptr, 0);
    mWritePos = 0;
}

void BufferWriter::reset(DataRef data) {
    mBuffer = data;
    mWritePos = 0;
}

DataRef BufferWriter::data() const { return DataRef(mBuffer.data(), size()); }

DataRef BufferWriter::total_data() const { return DataRef(mBuffer.data(), total_size()); }

DataRef BufferWriter::buffer() const { return mBuffer; }

size_t BufferWriter::size() const { return write_pos(); }

size_t BufferWriter::total_size() const { return write_pos(); }

size_t BufferWriter::buffer_size() const { return mBuffer.size(); }

void BufferWriter::write_pos(size_t pos) {
    if (pos <= buffer_size()) {
        mWritePos = pos;
    } else {
        mWritePos = size();
    }
}

size_t BufferWriter::write_pos() const { return mWritePos; }

int BufferWriter::write(DataRef data) {
    int err = write_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    mWritePos += data.size();
    return Success;
}

int BufferWriter::write_silent(DataRef data, size_t offset) {
    if (buffer_size() < offset + data.size()) {  // out of range
        return ErrorDataOutOfRange;
    }
    memcpy(mBuffer.data() + offset, data.data(), data.size());
    return Success;
}

int BufferWriter::write_char(char data) {
    int err = write_char_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    ++mWritePos;
    return Success;
}

int BufferWriter::write_char_silent(char data, size_t offset) {
    if (buffer_size() < offset + 1) {  // out of range
        return ErrorDataOutOfRange;
    }
    *(mBuffer.data() + offset) = data;
    return Success;
}

int BufferWriter::write_repeat_char(size_t count, char data) {
    int err = write_repeat_char_silent(count, data, write_pos());
    if (err != Success) {
        return err;
    }
    mWritePos += count;
    return Success;
}

int BufferWriter::write_repeat_char_silent(size_t count, char data, size_t offset) {
    if (buffer_size() < offset + count) {  // out of range
        return ErrorDataOutOfRange;
    }
    memset(mBuffer.data() + offset, data, count);
    return Success;
}

int BufferWriter::write_str(StringRef data) {
    int err = write_str_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    mWritePos += data.size();
    return Success;
}

int BufferWriter::write_str_silent(StringRef data, size_t offset) {
    if (buffer_size() < offset + data.size()) {  // out of range
        return ErrorDataOutOfRange;
    }
    memcpy(mBuffer.data() + offset, data.data(), data.size());
    return Success;
}

}  // namespace PureCore
