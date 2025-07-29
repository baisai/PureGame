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

#include "PureEncrypt/PureLz4.h"

#include "PureLua/LuaRegisterClass.h"

namespace PureApp {
void bind_encrypt_pure_lz4(lua_State* L) {
    using namespace PureEncrypt;
    PureLua::LuaModule lm(L, "PureEncrypt");
    lm[PureLua::LuaRegisterClass<PureLz4BlockEncoder>(L, "PureLz4BlockEncoder")
           .default_ctor()
           .def(&PureLz4BlockEncoder::set_block_size, "set_block_size")
           .def(&PureLz4BlockEncoder::get_block_size, "get_block_size")
           .def(&PureLz4BlockEncoder::set_encode_level, "set_encode_level")
           .def(&PureLz4BlockEncoder::get_encode_level, "get_encode_level")
           .def(&PureLz4BlockEncoder::encode, "encode")
           .def(&PureLz4BlockEncoder::start_encode, "start_encode")
           .def(&PureLz4BlockEncoder::update_encode, "update_encode")
           .def(&PureLz4BlockEncoder::flush_encode, "flush_encode")
           .def(&PureLz4BlockEncoder::finish_encode, "finish_encode")
           .def(&PureLz4BlockEncoder::get_output, "get_output")
           .def(&PureLz4BlockEncoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureLz4BlockDecoder>(L, "PureLz4BlockDecoder")
           .default_ctor()
           .def(&PureLz4BlockDecoder::decode, "decode")
           .def(&PureLz4BlockDecoder::start_decode, "start_decode")
           .def(&PureLz4BlockDecoder::update_decode, "update_decode")
           .def(&PureLz4BlockDecoder::finish_decode, "finish_decode")
           .def(&PureLz4BlockDecoder::get_output, "get_output")
           .def(&PureLz4BlockDecoder::clear, "clear") +
       PureLua::LuaRegisterClass<LZ4F_preferences_t>(L, "Lz4FramePrefer")
           .def_ctor([]() { return new LZ4F_preferences_t LZ4F_INIT_PREFERENCES; }, [](LZ4F_preferences_t* p) { delete p; })
           .def(&LZ4F_preferences_t::compressionLevel, "compressionLevel")
           .def(&LZ4F_preferences_t::autoFlush, "auto_flush")
           .def(&LZ4F_preferences_t::favorDecSpeed, "favorDecSpeed")
           .def([](LZ4F_preferences_t& self, int blockSizeID) { self.frameInfo.blockSizeID = LZ4F_blockSizeID_t(blockSizeID); }, "set_block_size_id")
           .def_const(LZ4F_default, "EBlockSizeDefault")
           .def_const(LZ4F_max64KB, "EBlockSizeMax64KB")
           .def_const(LZ4F_max256KB, "EBlockSizeMax256KB")
           .def_const(LZ4F_max1MB, "EBlockSizeMax1MB")
           .def_const(LZ4F_max4MB, "EBlockSizeMax4MB")
           .def([](LZ4F_preferences_t& self, int blockMode) { self.frameInfo.blockMode = LZ4F_blockMode_t(blockMode); }, "set_block_mode")
           .def_const(LZ4F_blockLinked, "EBlockModeLinked")
           .def_const(LZ4F_blockIndependent, "EBlockModeIndepen")
           .def([](LZ4F_preferences_t& self, int frameType) { self.frameInfo.frameType = LZ4F_frameType_t(frameType); }, "set_frame_type")
           .def_const(LZ4F_frame, "EFrameTypeFrame")
           .def_const(LZ4F_skippableFrame, "EFrameTypeSkippable")
           .def([](LZ4F_preferences_t& self, int checksumFlag) { self.frameInfo.blockChecksumFlag = LZ4F_blockChecksum_t(checksumFlag); },
                "set_block_checksum_flag")
           .def_const(LZ4F_noBlockChecksum, "EBlockChecksumNo")
           .def_const(LZ4F_blockChecksumEnabled, "EBlockChecksumYes")
           .def([](LZ4F_preferences_t& self) -> int { return self.frameInfo.blockSizeID; }, "get_block_size_id")
           .def([](LZ4F_preferences_t& self) -> int { return self.frameInfo.blockMode; }, "get_block_mode")
           .def([](LZ4F_preferences_t& self) -> int { return self.frameInfo.frameType; }, "get_frame_type")
           .def([](LZ4F_preferences_t& self) -> int { return self.frameInfo.blockChecksumFlag; }, "get_block_checksum_flag") +
       PureLua::LuaRegisterClass<PureLz4FrameEncoder>(L, "PureLz4FrameEncoder")
           .default_ctor()
           .def(&PureLz4FrameEncoder::set_preference, "set_preference")
           .def(&PureLz4FrameEncoder::encode, "encode")
           .def(&PureLz4FrameEncoder::start_encode, "start_encode")
           .def(&PureLz4FrameEncoder::update_encode, "update_encode")
           .def(&PureLz4FrameEncoder::finish_encode, "finish_encode")
           .def(&PureLz4FrameEncoder::get_lz4_error, "get_lz4_error")
           .def(&PureLz4FrameEncoder::get_output, "get_output")
           .def(&PureLz4FrameEncoder::clear, "clear") +
       PureLua::LuaRegisterClass<PureLz4FrameDecoder>(L, "PureLz4FrameDecoder")
           .default_ctor()
           .def(&PureLz4FrameDecoder::decode, "decode")
           .def(&PureLz4FrameDecoder::start_decode, "start_decode")
           .def(&PureLz4FrameDecoder::update_decode, "update_decode")
           .def(&PureLz4FrameDecoder::finish_decode, "finish_decode")
           .def(&PureLz4FrameDecoder::get_output, "get_output")
           .def(&PureLz4FrameDecoder::get_lz4_error, "get_lz4_error")
           .def(&PureLz4FrameDecoder::clear, "clear")];
}

}  // namespace PureApp
