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

#include "PureEncrypt/PureZip.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_encrypt_pure_zip(lua_State* L) {
    using namespace PureEncrypt;
    PureLua::LuaModule lm(L, "PureEncrypt");
    lm[PureLua::LuaRegisterClass<PureZipFileEncoder>(L, "PureZipFileEncoder")
           .default_ctor()
           .def(&PureZipFileEncoder::set_encode_level, "set_encode_level")
           .def(&PureZipFileEncoder::get_encode_level, "get_encode_level")
           .def(&PureZipFileEncoder::set_modify_date, "set_modify_date")
           .def(&PureZipFileEncoder::get_modify_date, "get_modify_date")
           .def(&PureZipFileEncoder::set_password, "set_password")
           .def(&PureZipFileEncoder::get_password, "get_password")
           .def(&PureZipFileEncoder::set_part_size, "set_part_size")
           .def(&PureZipFileEncoder::get_part_size, "get_part_size")
           .def(&PureZipFileEncoder::get_zip_error, "get_zip_error")
           .def(&PureZipFileEncoder::encode, "encode")
           .def(&PureZipFileEncoder::start_encode, "start_encode")
           .def(&PureZipFileEncoder::update_encode_entry_file, "update_encode_entry_file")
           .def(&PureZipFileEncoder::update_encode_entry_dir, "update_encode_entry_dir")
           .def(&PureZipFileEncoder::update_encode_entry, "update_encode_entry")
           .def(&PureZipFileEncoder::finish_encode, "finish_encode")
           .def(&PureZipFileEncoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureZipFileDecoder>(L, "PureZipFileDecoder")
           .default_ctor()
           .def(&PureZipFileDecoder::set_password, "set_password")
           .def(&PureZipFileDecoder::get_password, "get_password")
           .def(&PureZipFileDecoder::get_zip_error, "get_zip_error")
           .def(&PureZipFileDecoder::decode, "decode")
           .def(&PureZipFileDecoder::start_decode, "start_decode")
           .def(&PureZipFileDecoder::get_entries_count, "get_entries_count")
           .def(&PureZipFileDecoder::get_entry_name, "get_entry_name")
           .def(&PureZipFileDecoder::update_decode_entry_file, "update_decode_entry_file")
           .def(&PureZipFileDecoder::update_decode_entry, "update_decode_entry")
           .def(&PureZipFileDecoder::finish_decode, "finish_decode")
           .def(&PureZipFileDecoder::get_output, "get_output")
           .def(&PureZipFileDecoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureZipMemEncoder>(L, "PureZipMemEncoder")
           .default_ctor()
           .def(&PureZipMemEncoder::set_encode_level, "set_encode_level")
           .def(&PureZipMemEncoder::get_encode_level, "get_encode_level")
           .def(&PureZipMemEncoder::set_modify_date, "set_modify_date")
           .def(&PureZipMemEncoder::get_modify_date, "get_modify_date")
           .def(&PureZipMemEncoder::set_password, "set_password")
           .def(&PureZipMemEncoder::get_password, "get_password")
           .def(&PureZipMemEncoder::set_output_grow, "set_output_grow")
           .def(&PureZipMemEncoder::get_output_grow, "get_output_grow")
           .def(&PureZipMemEncoder::get_zip_error, "get_zip_error")
           .def(&PureZipMemEncoder::encode, "encode")
           .def(&PureZipMemEncoder::start_encode, "start_encode")
           .def(&PureZipMemEncoder::update_encode_entry_file, "update_encode_entry_file")
           .def(&PureZipMemEncoder::update_encode_entry_dir, "update_encode_entry_dir")
           .def(&PureZipMemEncoder::update_encode_entry, "update_encode_entry")
           .def(&PureZipMemEncoder::finish_encode, "finish_encode")
           .def(&PureZipMemEncoder::get_output, "get_output")
           .def(&PureZipMemEncoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureZipMemDecoder>(L, "PureZipMemDecoder")
           .default_ctor()
           .def(&PureZipMemDecoder::set_password, "set_password")
           .def(&PureZipMemDecoder::get_password, "get_password")
           .def(&PureZipMemDecoder::get_zip_error, "get_zip_error")
           .def(&PureZipMemDecoder::decode, "decode")
           .def(&PureZipMemDecoder::start_decode, "start_decode")
           .def(&PureZipMemDecoder::get_entries_count, "get_entries_count")
           .def(&PureZipMemDecoder::get_entry_name, "get_entry_name")
           .def(&PureZipMemDecoder::update_decode_entry_file, "update_decode_entry_file")
           .def(&PureZipMemDecoder::update_decode_entry, "update_decode_entry")
           .def(&PureZipMemDecoder::finish_decode, "finish_decode")
           .def(&PureZipMemDecoder::get_output, "get_output")
           .def(&PureZipMemDecoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureZipStreamEncoder>(L, "PureZipStreamEncoder")
           .default_ctor()
           .def(&PureZipStreamEncoder::set_block_size, "set_block_size")
           .def(&PureZipStreamEncoder::get_block_size, "get_block_size")
           .def(&PureZipStreamEncoder::set_encode_level, "set_encode_level")
           .def(&PureZipStreamEncoder::get_encode_level, "get_encode_level")
           .def(&PureZipStreamEncoder::get_zip_error, "get_zip_error")
           .def(&PureZipStreamEncoder::encode, "encode")
           .def(&PureZipStreamEncoder::start_encode, "start_encode")
           .def(&PureZipStreamEncoder::update_encode, "update_encode")
           .def(&PureZipStreamEncoder::flush_encode, "flush_encode")
           .def(&PureZipStreamEncoder::finish_encode, "finish_encode")
           .def(&PureZipStreamEncoder::get_output, "get_output")
           .def(&PureZipStreamEncoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureZipStreamDecoder>(L, "PureZipStreamDecoder")
           .default_ctor()
           .def(&PureZipStreamDecoder::get_zip_error, "get_zip_error")
           .def(&PureZipStreamDecoder::decode, "decode")
           .def(&PureZipStreamDecoder::start_decode, "start_decode")
           .def(&PureZipStreamDecoder::update_decode, "update_decode")
           .def(&PureZipStreamDecoder::finish_decode, "finish_decode")
           .def(&PureZipStreamDecoder::get_output, "get_output")
           .def(&PureZipStreamDecoder::clear, "clear")

    ];
}

}  // namespace PureApp
