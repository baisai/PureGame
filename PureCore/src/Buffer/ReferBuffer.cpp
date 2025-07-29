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
#include "PureCore/Buffer/ReferBuffer.h"

#include <stdlib.h>
#include <memory.h>

namespace PureCore {
ReferBuffer::ReferBuffer() : mView() {}

ReferBuffer::ReferBuffer(const ReferBuffer &cp) : mView(cp.mView) {}

ReferBuffer::ReferBuffer(DataRef buffer) : mView(buffer) {}

ReferBuffer::~ReferBuffer() { mView.clear(); }

ReferBuffer &ReferBuffer::operator=(const ReferBuffer &right) {
    mView = right.mView;
    return *this;
}

void ReferBuffer::clear() { mView.clear(); }

DataRef ReferBuffer::data() const { return mView.data(); }

DataRef ReferBuffer::total_data() const { return mView.total_data(); }

DataRef ReferBuffer::buffer() const { return mView.buffer(); }

DataRef ReferBuffer::free_buffer() const { return DataRef(mView.data().data() + total_size(), buffer_size() - total_size()); }

size_t ReferBuffer::size() const { return mView.size(); }

size_t ReferBuffer::total_size() const { return mView.total_size(); }

size_t ReferBuffer::buffer_size() const { return mView.buffer_size(); }

size_t ReferBuffer::free_size() const { return buffer_size() - total_size(); }

BufferView ReferBuffer::view() { return mView; }

void ReferBuffer::read_pos(size_t pos) { mView.read_pos(pos); }

size_t ReferBuffer::read_pos() const { return mView.read_pos(); }

int ReferBuffer::read(DataRef &data, size_t size) { return mView.read(data, size); }

int ReferBuffer::read_silent(DataRef &data, size_t size, size_t offset) { return mView.read_silent(data, size, offset); }

void ReferBuffer::write_pos(size_t pos) { mView.write_pos(pos); }

size_t ReferBuffer::write_pos() const { return mView.write_pos(); }

int ReferBuffer::write(DataRef data) { return mView.write(data); }

int ReferBuffer::write_silent(DataRef data, size_t offset) { return mView.write_silent(data, offset); }

int ReferBuffer::write_char(char data) { return mView.write_char(data); }

int ReferBuffer::write_char_silent(char data, size_t offset) { return mView.write_char_silent(data, offset); }

int ReferBuffer::write_repeat_char(size_t count, char data) { return mView.write_repeat_char(count, data); }

int ReferBuffer::write_repeat_char_silent(size_t count, char data, size_t offset) { return mView.write_repeat_char_silent(count, data, offset); }

int ReferBuffer::write_str(StringRef data) { return mView.write_str(data); }

int ReferBuffer::write_str_silent(StringRef data, size_t offset) { return mView.write_str_silent(data, offset); }

int ReferBuffer::resize_buffer(size_t size) {
    if (size <= mView.buffer_size()) {
        return Success;
    }
    return ErrorBufferNotEnough;
}

void ReferBuffer::reset_buffer(DataRef buffer) { mView.reset(buffer); }

}  // namespace PureCore
