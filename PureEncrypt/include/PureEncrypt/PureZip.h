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

#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureEncrypt/PureEncryptLib.h"

#include "zlib-ng.h"
#include "mz_zip.h"

namespace PureEncrypt {
class PUREENCRYPT_API PureZipFileEncoder {
public:
    PureZipFileEncoder();
    ~PureZipFileEncoder();

    int set_encode_level(int16_t level);
    int16_t get_encode_level() const;
    int set_modify_date(time_t date);
    time_t get_modify_date() const;
    int set_password(const char* pass);
    const std::string& get_password() const;
    int set_part_size(int64_t partSize);
    int64_t get_part_size() const;
    int32_t get_zip_error() const;

    int encode(const char* zipPath, const char* dirPath, bool append, bool recursive);

    int start_encode(const char* zipPath, bool append);
    int update_encode_entry_file(const char* name, const char* filePath);
    int update_encode_entry_dir(const char* rootPath, const char* dirPath, bool recursive);
    int update_encode_entry(const char* name, PureCore::DataRef data);
    int finish_encode();

    void clear();

private:
    void clear_hold_output();
    bool is_running() const;

private:
    void* mContext;
    int16_t mLevel;
    time_t mModifyDate;
    int64_t mPartSize;
    std::string mPass;
    int32_t mZipErr;

    PURE_DISABLE_COPY(PureZipFileEncoder)
};

class PUREENCRYPT_API PureZipFileDecoder {
public:
    PureZipFileDecoder();
    ~PureZipFileDecoder();

    int set_password(const char* pass);
    const std::string& get_password() const;
    int32_t get_zip_error() const;

    int decode(const char* zipPath, const char* dirPath);

    int start_decode(const char* zipPath);
    size_t get_entries_count();
    const char* get_entry_name(size_t idx) const;
    int update_decode_entry_file(const char* name, const char* filePath);
    int update_decode_entry(const char* name);
    int finish_decode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    void clear_hold_output();
    bool is_running() const;

private:
    void* mContext;
    std::vector<std::string> mEntries;
    std::string mPass;
    int32_t mZipErr;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureZipFileDecoder)
};

class PUREENCRYPT_API PureZipMemEncoder {
public:
    PureZipMemEncoder();
    ~PureZipMemEncoder();

    int set_encode_level(int16_t level);
    int16_t get_encode_level() const;
    int set_modify_date(time_t date);
    time_t get_modify_date() const;
    int set_password(const char* pass);
    const std::string& get_password() const;
    int set_output_grow(uint32_t grow);
    uint32_t get_output_grow() const;
    int32_t get_zip_error() const;

    int encode(const char* dirPath, bool recursive);

    int start_encode();
    int update_encode_entry_file(const char* name, const char* filePath);
    int update_encode_entry_dir(const char* rootPath, const char* dirPath, bool recursive);
    int update_encode_entry(const char* name, PureCore::DataRef data);
    int finish_encode();

    PureCore::DataRef get_output();

    void clear();

private:
    void clear_hold_output();
    bool is_running() const;

private:
    void* mContext;
    void* mOutput;
    uint32_t mOutputGrow;
    int16_t mLevel;
    time_t mModifyDate;
    std::string mPass;
    int32_t mZipErr;

    PURE_DISABLE_COPY(PureZipMemEncoder)
};

class PUREENCRYPT_API PureZipMemDecoder {
public:
    PureZipMemDecoder();
    ~PureZipMemDecoder();

    int set_password(const char* pass);
    const std::string& get_password() const;
    int32_t get_zip_error() const;

    int decode(PureCore::DataRef data, const char* dirPath);

    int start_decode(PureCore::DataRef data);
    size_t get_entries_count();
    const char* get_entry_name(size_t idx) const;
    int update_decode_entry_file(const char* name, const char* filePath);
    int update_decode_entry(const char* name);
    int finish_decode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    void clear_hold_output();
    bool is_running() const;

private:
    void* mContext;
    void* mCache;
    std::vector<std::string> mEntries;
    std::string mPass;
    int32_t mZipErr;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureZipMemDecoder)
};

class PUREENCRYPT_API PureZipStreamEncoder {
public:
    PureZipStreamEncoder();
    ~PureZipStreamEncoder();

    int set_block_size(uint32_t size);
    uint32_t get_block_size() const;
    int set_encode_level(int16_t level);
    int16_t get_encode_level() const;
    int32_t get_zip_error() const;

    int encode(PureCore::DataRef src);

    int start_encode();
    int update_encode(PureCore::DataRef src);
    int flush_encode();
    int finish_encode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    int encode_data(PureCore::DataRef src, bool isEnd);
    void clear_hold_output();
    bool is_running() const;

private:
    zng_stream* mContext;
    uint32_t mBlockSize;
    int16_t mLevel;
    int32_t mZipErr;

    PureCore::DynamicBuffer mInputCache;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureZipStreamEncoder)
};

class PUREENCRYPT_API PureZipStreamDecoder {
public:
    PureZipStreamDecoder();
    ~PureZipStreamDecoder();

    int32_t get_zip_error() const;

    int decode(PureCore::DataRef src);

    int start_decode();
    int update_decode(PureCore::DataRef src);
    int finish_decode();

    PureCore::DynamicBuffer& get_output();

    void clear();

private:
    int decode_data(PureCore::DataRef src);
    void clear_hold_output();

private:
    zng_stream* mContext;
    uint32_t mBlockSize;
    uint32_t mNeedSize;
    int mDecodeStep;
    int32_t mZipErr;

    PureCore::DynamicBuffer mInputCache;
    PureCore::DynamicBuffer mOutput;

    PURE_DISABLE_COPY(PureZipStreamDecoder)
};

}  // namespace PureEncrypt
