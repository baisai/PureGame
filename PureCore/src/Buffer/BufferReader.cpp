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
#include "PureCore/Buffer/BufferReader.h"

namespace PureCore {
BufferReader::BufferReader() : mBuffer(), mReadPos(0) {}

BufferReader::BufferReader(const BufferReader& cp) : mBuffer(cp.mBuffer), mReadPos(cp.mReadPos) {}

BufferReader::BufferReader(DataRef buffer) : mBuffer(buffer), mReadPos(0) {}

BufferReader::~BufferReader() {
    mBuffer.reset(nullptr, 0);
    mReadPos = 0;
}

BufferReader& BufferReader::operator=(const BufferReader& right) {
    mBuffer = right.mBuffer;
    mReadPos = right.mReadPos;
    return *this;
}

void BufferReader::clear() {
    mBuffer.reset(nullptr, 0);
    mReadPos = 0;
}

void BufferReader::reset(DataRef buffer) {
    mBuffer = buffer;
    mReadPos = 0;
}

DataRef BufferReader::data() const { return DataRef(mBuffer.data() + read_pos(), size()); }

DataRef BufferReader::total_data() const { return mBuffer; }

DataRef BufferReader::buffer() const { return mBuffer; }

size_t BufferReader::size() const { return buffer_size() - read_pos(); }

size_t BufferReader::total_size() const { return mBuffer.size(); }

size_t BufferReader::buffer_size() const { return mBuffer.size(); }

void BufferReader::read_pos(size_t pos) {
    if (pos <= buffer_size()) {
        mReadPos = pos;
    } else {
        mReadPos = buffer_size();
    }
}

size_t BufferReader::read_pos() const { return mReadPos; }

int BufferReader::read(DataRef& data, size_t size) {
    int err = read_silent(data, size, read_pos());
    if (err != Success) {
        return err;
    }
    mReadPos += size;
    return Success;
}

int BufferReader::read_silent(DataRef& data, size_t size, size_t offset) {
    if (this->buffer_size() < offset + size) {  // out of range
        return ErrorDataOutOfRange;
    }

    data.reset(mBuffer.data() + offset, size);
    return Success;
}
}  // namespace PureCore
