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

#include "PureCore/Buffer/ArrayBuffer.h"
#include "PureCore/CoreErrorDesc.h"

namespace PureCore {
template <size_t TSize>
ArrayBuffer<TSize>::ArrayBuffer() : mBuffer{}, mView(DataRef(mBuffer, TSize)) {}

template <size_t TSize>
ArrayBuffer<TSize>::ArrayBuffer(const ArrayBuffer &cp) : mBuffer{}, mView(DataRef(mBuffer, TSize)) {
    write(cp.data());
}

template <size_t TSize>
ArrayBuffer<TSize>::~ArrayBuffer() {
    mView.clear();
}

template <size_t TSize>
ArrayBuffer<TSize> &ArrayBuffer<TSize>::operator=(const ArrayBuffer &right) {
    memcpy(mBuffer, right.mBuffer, right.write_pos());
    read_pos(right.read_pos());
    write_pos(right.write_pos());
    return *this;
}

template <size_t TSize>
void ArrayBuffer<TSize>::clear() {
    mView.read_pos(0);
    mView.write_pos(0);
}

template <size_t TSize>
DataRef ArrayBuffer<TSize>::data() const {
    return mView.data();
}

template <size_t TSize>
DataRef ArrayBuffer<TSize>::total_data() const {
    return mView.total_data();
}

template <size_t TSize>
DataRef ArrayBuffer<TSize>::buffer() const {
    return DataRef(mBuffer, TSize);
}

template <size_t TSize>
DataRef ArrayBuffer<TSize>::free_buffer() const {
    return DataRef(mBuffer + total_size(), buffer_size() - total_size());
}

template <size_t TSize>
size_t ArrayBuffer<TSize>::size() const {
    return mView.size();
}

template <size_t TSize>
size_t ArrayBuffer<TSize>::total_size() const {
    return mView.total_size();
}

template <size_t TSize>
size_t ArrayBuffer<TSize>::buffer_size() const {
    return TSize;
}

template <size_t TSize>
size_t ArrayBuffer<TSize>::free_size() const {
    return buffer_size() - total_size();
}

template <size_t TSize>
BufferView ArrayBuffer<TSize>::view() {
    return mView;
}

template <size_t TSize>
void ArrayBuffer<TSize>::read_pos(size_t pos) {
    mView.read_pos(pos);
}

template <size_t TSize>
size_t ArrayBuffer<TSize>::read_pos() const {
    return mView.read_pos();
}

template <size_t TSize>
int ArrayBuffer<TSize>::read(DataRef &data, size_t size) {
    return mView.read(data, size);
}

template <size_t TSize>
int ArrayBuffer<TSize>::read_silent(DataRef &data, size_t size, size_t offset) {
    return mView.read_silent(data, size, offset);
}

template <size_t TSize>
void ArrayBuffer<TSize>::write_pos(size_t pos) {
    mView.write_pos(pos);
}

template <size_t TSize>
size_t ArrayBuffer<TSize>::write_pos() const {
    return mView.write_pos();
}

template <size_t TSize>
int ArrayBuffer<TSize>::write(DataRef data) {
    return mView.write(data);
}

template <size_t TSize>
int ArrayBuffer<TSize>::write_silent(DataRef data, size_t offset) {
    return mView.write_silent(data, offset);
}

template <size_t TSize>
int ArrayBuffer<TSize>::write_char(char data) {
    return mView.write_char(data);
}

template <size_t TSize>
int ArrayBuffer<TSize>::write_char_silent(char data, size_t offset) {
    return mView.write_char_silent(data, offset);
}

template <size_t TSize>
int ArrayBuffer<TSize>::write_repeat_char(size_t count, char data) {
    return mView.write_repeat_char(count, data);
}

template <size_t TSize>
int ArrayBuffer<TSize>::write_repeat_char_silent(size_t count, char data, size_t offset) {
    return mView.write_repeat_char_silent(count, data, offset);
}

template <size_t TSize>
int ArrayBuffer<TSize>::write_str(StringRef data) {
    return mView.write_str(data);
}

template <size_t TSize>
int ArrayBuffer<TSize>::write_str_silent(StringRef data, size_t offset) {
    return mView.write_str_silent(data, offset);
}

template <size_t TSize>
int ArrayBuffer<TSize>::resize_buffer(size_t size) {
    if (size <= TSize) {
        return Success;
    }
    return ErrorBufferNotEnough;
}

}  // namespace PureCore
