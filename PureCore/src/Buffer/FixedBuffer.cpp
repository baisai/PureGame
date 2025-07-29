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

#include "PureCore/Buffer/FixedBuffer.h"
#include "PureCore/CoreErrorDesc.h"

namespace PureCore {
FixedBuffer::FixedBuffer(size_t size) : mBuffer(), mView() {
    mBuffer.reset((const char *)::malloc(size), size);
    mView.reset(mBuffer);
}

FixedBuffer::FixedBuffer(const FixedBuffer &cp) : mBuffer(), mView() {
    mBuffer.reset((const char *)::malloc(cp.buffer_size()), cp.buffer_size());
    mView.reset(mBuffer);
    write(cp.data());
}

FixedBuffer::FixedBuffer(FixedBuffer &&cp) : mBuffer(cp.mBuffer), mView(cp.mView) {
    cp.mBuffer.reset(nullptr, 0);
    cp.mView.clear();
}

FixedBuffer::~FixedBuffer() {
    if (!mBuffer.empty()) {
        ::free(mBuffer.data());
        mBuffer.reset(nullptr, 0);
        mView.clear();
    }
}

FixedBuffer &FixedBuffer::operator=(const FixedBuffer &right) {
    ::free(mBuffer.data());
    mBuffer.reset((const char *)::malloc(right.buffer_size()), right.buffer_size());
    mView.reset(mBuffer);
    write(right.data());
    return *this;
}

FixedBuffer &FixedBuffer::operator=(FixedBuffer &&right) {
    if (!mBuffer.empty()) {
        ::free(mBuffer.data());
    }
    mBuffer = right.mBuffer;
    mView = right.mView;
    right.mBuffer.reset(nullptr, 0);
    right.mView.clear();
    return *this;
}

void FixedBuffer::swap(FixedBuffer &dest) {
    PureCore::DataRef buffer = mBuffer;
    BufferView view = mView;
    mBuffer = dest.mBuffer;
    mView = dest.mView;
    dest.mBuffer = buffer;
    dest.mView = view;
}

void FixedBuffer::clear() {
    mView.read_pos(0);
    mView.write_pos(0);
}

DataRef FixedBuffer::data() const { return mView.data(); }

DataRef FixedBuffer::total_data() const { return mView.total_data(); }

DataRef FixedBuffer::buffer() const { return mBuffer; }

DataRef FixedBuffer::free_buffer() const { return DataRef(mBuffer.data() + total_size(), buffer_size() - total_size()); }

size_t FixedBuffer::size() const { return mView.size(); }

size_t FixedBuffer::total_size() const { return mView.total_size(); }

size_t FixedBuffer::buffer_size() const { return mBuffer.size(); }

size_t FixedBuffer::free_size() const { return buffer_size() - total_size(); }

BufferView FixedBuffer::view() { return mView; }

void FixedBuffer::read_pos(size_t pos) { mView.read_pos(pos); }

size_t FixedBuffer::read_pos() const { return mView.read_pos(); }

int FixedBuffer::read(DataRef &data, size_t size) { return mView.read(data, size); }

int FixedBuffer::read_silent(DataRef &data, size_t size, size_t offset) { return mView.read_silent(data, size, offset); }

void FixedBuffer::write_pos(size_t pos) { mView.write_pos(pos); }

size_t FixedBuffer::write_pos() const { return mView.write_pos(); }

int FixedBuffer::write(DataRef data) { return mView.write(data); }

int FixedBuffer::write_silent(DataRef data, size_t offset) { return mView.write_silent(data, offset); }

int FixedBuffer::write_char(char data) { return mView.write_char(data); }

int FixedBuffer::write_char_silent(char data, size_t offset) { return mView.write_char_silent(data, offset); }

int FixedBuffer::write_repeat_char(size_t count, char data) { return mView.write_repeat_char(count, data); }

int FixedBuffer::write_repeat_char_silent(size_t count, char data, size_t offset) { return mView.write_repeat_char_silent(count, data, offset); }

int FixedBuffer::write_str(StringRef data) { return mView.write_str(data); }

int FixedBuffer::write_str_silent(StringRef data, size_t offset) { return mView.write_str_silent(data, offset); }

int FixedBuffer::resize_buffer(size_t size) {
    if (size <= buffer_size()) {
        return Success;
    }
    return ErrorBufferNotEnough;
}

}  // namespace PureCore
