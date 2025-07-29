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
#include "PureCore/Buffer/DynamicBuffer.h"

#include <stdlib.h>
#include <memory.h>

namespace PureCore {
DynamicBuffer::DynamicBuffer() : mBuffer(), mView() {}

DynamicBuffer::DynamicBuffer(const DynamicBuffer &cp) : mBuffer(), mView() {
    int err = resize_buffer(cp.buffer_size());
    if (err != Success) {
        return;
    }
    memcpy(mBuffer.data(), cp.mBuffer.data(), cp.mBuffer.size());
    mView.read_pos(cp.mView.read_pos());
    mView.write_pos(cp.mView.write_pos());
}

DynamicBuffer::DynamicBuffer(DynamicBuffer &&cp) : mBuffer(cp.mBuffer), mView(cp.mView) {
    cp.mBuffer.reset(nullptr, 0);
    cp.mView.clear();
}

DynamicBuffer::DynamicBuffer(size_t size) : mBuffer(), mView() { resize_buffer(size); }

DynamicBuffer::~DynamicBuffer() {
    if (!mBuffer.empty()) {
        ::free(mBuffer.data());
        mBuffer.reset(nullptr, 0);
        mView.clear();
    }
}

DynamicBuffer &DynamicBuffer::operator=(const DynamicBuffer &right) {
    int err = resize_buffer(right.buffer_size());
    if (err != Success) {
        return *this;
    }
    memcpy(mBuffer.data(), right.mBuffer.data(), right.mBuffer.size());
    mView.read_pos(right.mView.read_pos());
    mView.write_pos(right.mView.write_pos());
    return *this;
}

DynamicBuffer &DynamicBuffer::operator=(DynamicBuffer &&right) {
    if (!mBuffer.empty()) {
        ::free(mBuffer.data());
    }
    mBuffer = right.mBuffer;
    mView = right.mView;
    right.mBuffer.reset(nullptr, 0);
    right.mView.clear();
    return *this;
}

void DynamicBuffer::swap(DynamicBuffer &dest) {
    PureCore::DataRef buffer = mBuffer;
    BufferView view = mView;
    mBuffer = dest.mBuffer;
    mView = dest.mView;
    dest.mBuffer = buffer;
    dest.mView = view;
}

void DynamicBuffer::clear() {
    mView.read_pos(0);
    mView.write_pos(0);
}

DataRef DynamicBuffer::data() const { return mView.data(); }

DataRef DynamicBuffer::total_data() const { return mView.total_data(); }

DataRef DynamicBuffer::buffer() const { return mBuffer; }

DataRef DynamicBuffer::free_buffer() const { return DataRef(mBuffer.data() + total_size(), buffer_size() - total_size()); }

size_t DynamicBuffer::size() const { return mView.size(); }

size_t DynamicBuffer::total_size() const { return mView.total_size(); }

size_t DynamicBuffer::buffer_size() const { return mBuffer.size(); }

size_t DynamicBuffer::free_size() const { return buffer_size() - total_size(); }

BufferView DynamicBuffer::view() { return mView; }

void DynamicBuffer::read_pos(size_t pos) { mView.read_pos(pos); }

size_t DynamicBuffer::read_pos() const { return mView.read_pos(); }

int DynamicBuffer::read(DataRef &data, size_t size) { return mView.read(data, size); }

int DynamicBuffer::read_silent(DataRef &data, size_t size, size_t offset) { return mView.read_silent(data, size, offset); }

void DynamicBuffer::write_pos(size_t pos) { mView.write_pos(pos); }

size_t DynamicBuffer::write_pos() const { return mView.write_pos(); }

int DynamicBuffer::write(DataRef data) {
    int err = write_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    mView.write_pos(mView.write_pos() + data.size());
    return Success;
}

int DynamicBuffer::write_silent(DataRef data, size_t offset) {
    int err = ensure_buffer(offset + data.size());
    if (err != Success) {
        return err;
    }
    return mView.write_silent(data, offset);
}

int DynamicBuffer::write_char(char data) {
    int err = write_char_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    mView.write_pos(mView.write_pos() + 1);
    return Success;
}

int DynamicBuffer::write_char_silent(char data, size_t offset) {
    int err = ensure_buffer(offset + 1);
    if (err != Success) {
        return err;
    }
    return mView.write_char_silent(data, offset);
}

int DynamicBuffer::write_repeat_char(size_t count, char data) {
    int err = write_repeat_char_silent(count, data, write_pos());
    if (err != Success) {
        return err;
    }
    mView.write_pos(mView.write_pos() + count);
    return Success;
}

int DynamicBuffer::write_repeat_char_silent(size_t count, char data, size_t offset) {
    int err = ensure_buffer(offset + count);
    if (err != Success) {
        return err;
    }
    return mView.write_repeat_char_silent(count, data, offset);
}

int DynamicBuffer::write_str(StringRef data) {
    int err = write_str_silent(data, write_pos());
    if (err != Success) {
        return err;
    }
    mView.write_pos(mView.write_pos() + data.size());
    return Success;
}

int DynamicBuffer::write_str_silent(StringRef data, size_t offset) {
    int err = ensure_buffer(offset + data.size());
    if (err != Success) {
        return err;
    }
    return mView.write_str_silent(data, offset);
}

int DynamicBuffer::resize_buffer(size_t size) {
    if (mBuffer.size() >= size) {
        return Success;
    }

    char *newBuffer = (char *)::realloc(mBuffer.data(), size);
    if (!newBuffer) {
        return ErrorMemoryNotEnough;
    }
    size_t readPos = mView.read_pos();
    size_t writePos = mView.write_pos();
    mBuffer.reset(newBuffer, size);
    mView.reset(mBuffer);
    if (writePos > size) {
        writePos = size;
    }
    if (readPos > writePos) {
        readPos = writePos;
    }
    mView.write_pos(writePos);
    mView.read_pos(readPos);
    return Success;
}

int DynamicBuffer::ensure_buffer(size_t size) {
    if (size <= buffer_size()) {
        return Success;
    }
    return grow_buffer(size - buffer_size());
}

void DynamicBuffer::align_data() {
    if (read_pos() == 0) {
        return;
    }
    if (size() == 0) {
        clear();
        return;
    }
    auto d = data();
    memmove(mBuffer.data(), d.data(), d.size());
    mView.read_pos(0);
    mView.write_pos(d.size());
}

int DynamicBuffer::grow_buffer(size_t size) {
    if (size <= 0) {
        return Success;
    }
    size_t bufferSize = buffer_size();
    bufferSize += ((bufferSize >> 1) + size + 16);
    return resize_buffer(bufferSize);
}

}  // namespace PureCore
