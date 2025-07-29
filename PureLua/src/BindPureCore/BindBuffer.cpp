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
#include "PureCore/Buffer/BufferWriter.h"
#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureCore/Buffer/FixedBuffer.h"
#include "PureCore/Buffer/ReferBuffer.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureLua {
template <typename T, bool Silent>
static int wrap_read(lua_State* L) {
    T& self = PureLua::LuaStack<T&>::get(L, 1);
    size_t s = PureLua::LuaStack<size_t>::get(L, 2);
    PureCore::DataRef data;
    int err = 0;
    if constexpr (Silent) {
        err = self.read_silent(data, s);
        if (err != PureCore::Success) {
            PureLuaErrorJump(L, "read silent failed `{}`", PureCore::get_error_desc(err));
            return 0;
        }
    } else {
        err = self.read(data, s);
        if (err != PureCore::Success) {
            PureLuaErrorJump(L, "read failed `{}`", PureCore::get_error_desc(err));
            return 0;
        }
    }
    PureLua::LuaStack<PureCore::DataRef>::push(L, data);
    return 1;
}

void bind_core_buffer(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule lm(L, "PureCore");
    lm[PureLua::LuaRegisterClass<IBuffer>(L, "IBuffer")
           .def(&IBuffer::clear, "clear")
           .def(&IBuffer::data, "data")
           .def(&IBuffer::total_data, "total_data")
           .def(&IBuffer::buffer, "buffer")
           .def(&IBuffer::free_buffer, "free_buffer")
           .def(&IBuffer::size, "size")
           .def(&IBuffer::total_size, "total_size")
           .def(&IBuffer::buffer_size, "buffer_size")
           .def(&IBuffer::free_size, "free_size")
           .def(&IBuffer::view, "view")
           .def(static_cast<void (IBuffer::*)(size_t)>(&IBuffer::read_pos), "set_read_pos")
           .def(static_cast<size_t (IBuffer::*)() const>(&IBuffer::read_pos), "get_read_pos")
           .def(wrap_read<IBuffer, false>, "read")
           .def(wrap_read<IBuffer, true>, "read_silent")
           .def(static_cast<void (IBuffer::*)(size_t)>(&IBuffer::write_pos), "set_write_pos")
           .def(static_cast<size_t (IBuffer::*)() const>(&IBuffer::write_pos), "get_write_pos")
           .def(&IBuffer::write, "write")
           .def(&IBuffer::write_silent, "write_silent")
           .def(&IBuffer::write_char, "write_char")
           .def(&IBuffer::write_char_silent, "write_char_silent")
           .def(&IBuffer::write_repeat_char, "write_repeat_char")
           .def(&IBuffer::write_repeat_char_silent, "write_repeat_char_silent")
           .def(&IBuffer::write_str, "write_str")
           .def(&IBuffer::write_str_silent, "write_str_silent")
           .def(&IBuffer::resize_buffer, "resize_buffer") +
       PureLua::LuaRegisterClass<BufferView>(L, "BufferView")
           .default_ctor<DataRef>()
           .def(&BufferView::clear, "clear")
           .def(&BufferView::reset, "reset")
           .def(&BufferView::data, "data")
           .def(&BufferView::total_data, "total_data")
           .def(&BufferView::buffer, "buffer")
           .def(&BufferView::size, "size")
           .def(&BufferView::total_size, "total_size")
           .def(&BufferView::buffer_size, "buffer_size")
           .def(static_cast<void (BufferView::*)(size_t)>(&BufferView::read_pos), "set_read_pos")
           .def(static_cast<size_t (BufferView::*)() const>(&BufferView::read_pos), "get_read_pos")
           .def(wrap_read<BufferView, false>, "read")
           .def(wrap_read<BufferView, true>, "read_silent")
           .def(static_cast<void (BufferView::*)(size_t)>(&BufferView::write_pos), "set_write_pos")
           .def(static_cast<size_t (BufferView::*)() const>(&BufferView::write_pos), "get_write_pos")
           .def(&BufferView::write, "write")
           .def(&BufferView::write_silent, "write_silent")
           .def(&BufferView::write_char, "write_char")
           .def(&BufferView::write_char_silent, "write_char_silent")
           .def(&BufferView::write_repeat_char, "write_repeat_char")
           .def(&BufferView::write_repeat_char_silent, "write_repeat_char_silent")
           .def(&BufferView::write_str, "write_str")
           .def(&BufferView::write_str_silent, "write_str_silent") +
       PureLua::LuaRegisterClass<BufferReader>(L, "BufferReader")
           .default_ctor<DataRef>()
           .def(&BufferReader::clear, "clear")
           .def(&BufferReader::reset, "reset")
           .def(&BufferReader::data, "data")
           .def(&BufferReader::total_data, "total_data")
           .def(&BufferReader::buffer, "buffer")
           .def(&BufferReader::size, "size")
           .def(&BufferReader::total_size, "total_size")
           .def(&BufferReader::buffer_size, "buffer_size")
           .def(static_cast<void (BufferReader::*)(size_t)>(&BufferReader::read_pos), "set_read_pos")
           .def(static_cast<size_t (BufferReader::*)() const>(&BufferReader::read_pos), "get_read_pos")
           .def(wrap_read<BufferReader, false>, "read")
           .def(wrap_read<BufferReader, true>, "read_silent") +
       PureLua::LuaRegisterClass<BufferWriter>(L, "BufferWriter")
           .default_ctor<DataRef>()
           .def(&BufferWriter::clear, "clear")
           .def(&BufferWriter::reset, "reset")
           .def(&BufferWriter::data, "data")
           .def(&BufferWriter::total_data, "total_data")
           .def(&BufferWriter::buffer, "buffer")
           .def(&BufferWriter::size, "size")
           .def(&BufferWriter::total_size, "total_size")
           .def(&BufferWriter::buffer_size, "buffer_size")
           .def(static_cast<void (BufferWriter::*)(size_t)>(&BufferWriter::write_pos), "set_write_pos")
           .def(static_cast<size_t (BufferWriter::*)() const>(&BufferWriter::write_pos), "get_write_pos")
           .def(&BufferWriter::write, "write")
           .def(&BufferWriter::write_silent, "write_silent")
           .def(&BufferWriter::write_char, "write_char")
           .def(&BufferWriter::write_char_silent, "write_char_silent")
           .def(&BufferWriter::write_repeat_char, "write_repeat_char")
           .def(&BufferWriter::write_repeat_char_silent, "write_repeat_char_silent")
           .def(&BufferWriter::write_str, "write_str")
           .def(&BufferWriter::write_str_silent, "write_str_silent")];
    lm[PureLua::LuaRegisterClass<FixedBuffer>(L, "FixedBuffer", PureLua::BaseClassStrategy<IBuffer>()).default_ctor<size_t>().def(&FixedBuffer::swap, "swap") +
       PureLua::LuaRegisterClass<ReferBuffer>(L, "ReferBuffer", PureLua::BaseClassStrategy<IBuffer>())
           .default_ctor<DataRef>()
           .def(&ReferBuffer::reset_buffer, "reset_buffer") +
       PureLua::LuaRegisterClass<DynamicBuffer>(L, "DynamicBuffer", PureLua::BaseClassStrategy<IBuffer>())
           .default_ctor()
           .def(&DynamicBuffer::swap, "swap")
           .def(&DynamicBuffer::ensure_buffer, "ensure_buffer")
           .def(&DynamicBuffer::align_data, "align_data")];
}

}  // namespace PureLua
