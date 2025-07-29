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

#include "PureCore/PureLog.h"
#include "PureCore/DataRef.h"
#include "PureCore/OsHelper.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureEncrypt/PureZip.h"

#include "mz_os.h"
#include "mz.h"
#include "mz_strm.h"
#include "mz_strm_mem.h"
#include "mz_strm_buf.h"
#include "mz_zip_rw.h"

namespace PureEncrypt {
/////////////////////////////////////////////////////////////////
/// PureZipFileEncoder
///////////////////////////////////////////////////////////////
PureZipFileEncoder::PureZipFileEncoder() : mContext(nullptr), mLevel(MZ_COMPRESS_LEVEL_NORMAL), mModifyDate(0), mPartSize(0), mPass(), mZipErr(MZ_OK) {}
PureZipFileEncoder::~PureZipFileEncoder() { clear(); }

int PureZipFileEncoder::set_encode_level(int16_t level) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mLevel = level;
    return Success;
}

int16_t PureZipFileEncoder::get_encode_level() const { return mLevel; }

int PureZipFileEncoder::set_modify_date(time_t date) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mModifyDate = date;
    return Success;
}

time_t PureZipFileEncoder::get_modify_date() const { return mModifyDate; }

int PureZipFileEncoder::set_password(const char* pass) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mPass = pass;
    return Success;
}

const std::string& PureZipFileEncoder::get_password() const { return mPass; }

int PureZipFileEncoder::set_part_size(int64_t partSize) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mPartSize = partSize;
    return Success;
}

int64_t PureZipFileEncoder::get_part_size() const { return mPartSize; }

int32_t PureZipFileEncoder::get_zip_error() const { return mZipErr; }

int PureZipFileEncoder::encode(const char* zipPath, const char* dirPath, bool append, bool recursive) {
    int err = start_encode(zipPath, append);
    if (err != Success) {
        return err;
    }

    err = update_encode_entry_dir(dirPath, dirPath, recursive);
    if (err != Success) {
        return err;
    }
    return finish_encode();
}

int PureZipFileEncoder::start_encode(const char* zipPath, bool append) {
    clear();
    mContext = mz_zip_writer_create();
    if (mContext == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return ErrorZipOpenFailed;
    }
    mz_zip_writer_set_compress_method(mContext, MZ_COMPRESS_METHOD_DEFLATE);
    if (!mPass.empty()) {
#if defined(PURE_OPENSSL) && PURE_OPENSSL
        mz_zip_writer_set_password(mContext, mPass.c_str());
        mz_zip_writer_set_aes(mContext, 1);
#else
        return ErrorNotSupport;
#endif
    }
    int32_t err = mz_zip_writer_open_file(mContext, zipPath, mPartSize, append ? 1 : 0);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }
    return Success;
}

int PureZipFileEncoder::update_encode_entry_file(const char* name, const char* filePath) {
    if (mContext == nullptr) {
        mZipErr = MZ_PARAM_ERROR;
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_writer_add_file(mContext, filePath, name);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipFileEncoder::update_encode_entry_dir(const char* rootPath, const char* dirPath, bool recursive) {
    if (mContext == nullptr) {
        mZipErr = MZ_PARAM_ERROR;
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_writer_add_path(mContext, dirPath, rootPath, 0, recursive ? 1 : 0);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipFileEncoder::update_encode_entry(const char* name, PureCore::DataRef data) {
    if (mContext == nullptr) {
        mZipErr = MZ_PARAM_ERROR;
        return ErrorZipContextError;
    }
    mz_zip_file fileInfo = {0};
    fileInfo.filename = name;
    fileInfo.creation_date = mModifyDate;
    fileInfo.modified_date = mModifyDate;
    fileInfo.accessed_date = mModifyDate;
    fileInfo.version_madeby = MZ_VERSION_MADEBY;
    fileInfo.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    fileInfo.uncompressed_size = data.size();
    fileInfo.flag = MZ_ZIP_FLAG_UTF8;
#if defined(PURE_OPENSSL) && PURE_OPENSSL
    fileInfo.aes_version = MZ_AES_VERSION;
#endif
    int32_t err = mz_zip_writer_add_buffer(mContext, data.data(), (int32_t)data.size(), &fileInfo);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipFileEncoder::finish_encode() {
    clear_hold_output();
    return Success;
}

void PureZipFileEncoder::clear() {
    clear_hold_output();
    mZipErr = MZ_OK;
}

void PureZipFileEncoder::clear_hold_output() {
    if (mContext != nullptr) {
        if (mz_zip_writer_is_open(mContext) == MZ_OK) {
            mz_zip_writer_close(mContext);
        }
        mz_zip_writer_delete(&mContext);
    }
}

bool PureZipFileEncoder::is_running() const { return mContext != nullptr; }

/////////////////////////////////////////////////////////////////
/// PureZipFileDecoder
///////////////////////////////////////////////////////////////
PureZipFileDecoder::PureZipFileDecoder() : mContext(nullptr), mEntries(), mPass(), mZipErr(MZ_OK), mOutput() {}

PureZipFileDecoder::~PureZipFileDecoder() { clear(); }

int PureZipFileDecoder::set_password(const char* pass) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mPass = pass;
    return Success;
}

const std::string& PureZipFileDecoder::get_password() const { return mPass; }

int32_t PureZipFileDecoder::get_zip_error() const { return mZipErr; }

int PureZipFileDecoder::decode(const char* zipPath, const char* dirPath) {
    if (!PureCore::file_exist(zipPath)) {
        return ErrorZipSrcNotExist;
    }
    if (!PureCore::dir_exist(dirPath)) {
        return ErrorPathNotExist;
    }
    int err = start_decode(zipPath);
    if (err != Success) {
        return err;
    }
    size_t count = get_entries_count();
    if (count <= 0) {
        return ErrorZipEntryFailed;
    }

    std::string parentPath(dirPath);
    if (parentPath.empty()) {
        parentPath = "./";
    } else if (parentPath.back() != '/' || parentPath.back() != '\\') {
        parentPath.push_back('/');
    }
    for (size_t i = 0; i < count; ++i) {
        const char* entry = get_entry_name(i);
        if (entry == nullptr) {
            return ErrorZipEntryFailed;
        }
        std::string file(parentPath + entry);
        int err = update_decode_entry_file(entry, file.c_str());
        if (err != Success) {
            return err;
        }
    }
    return finish_decode();
}

int PureZipFileDecoder::start_decode(const char* zipPath) {
    clear();
    mContext = mz_zip_reader_create();
    if (mContext == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return ErrorZipOpenFailed;
    }
    if (!mPass.empty()) {
        mz_zip_reader_set_password(mContext, mPass.c_str());
    }
    int32_t err = mz_zip_reader_open_file(mContext, zipPath);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }

    err = mz_zip_reader_goto_first_entry(mContext);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }

    mEntries.clear();
    do {
        mz_zip_file* fileInfo = nullptr;
        err = mz_zip_reader_entry_get_info(mContext, &fileInfo);
        if (err != MZ_OK) {
            mZipErr = err;
            return ErrorZipOpenFailed;
        }
        if (fileInfo != nullptr && fileInfo->filename != nullptr) {
            mEntries.push_back(fileInfo->filename);
        }
    } while (mz_zip_reader_goto_next_entry(mContext) == MZ_OK);
    return Success;
}

size_t PureZipFileDecoder::get_entries_count() {
    if (mContext == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return 0;
    }
    if (mz_zip_reader_is_open(mContext) != MZ_OK) {
        mZipErr = MZ_OPEN_ERROR;
        return 0;
    }
    return int32_t(mEntries.size());
}

const char* PureZipFileDecoder::get_entry_name(size_t idx) const {
    if (mContext == nullptr || idx >= mEntries.size()) {
        return nullptr;
    }
    return mEntries[idx].c_str();
}

int PureZipFileDecoder::update_decode_entry_file(const char* name, const char* filePath) {
    if (mContext == nullptr) {
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_reader_locate_entry(mContext, name, 1);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }

    err = mz_zip_reader_entry_save_file(mContext, filePath);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipFileDecoder::update_decode_entry(const char* name) {
    if (mContext == nullptr) {
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_reader_locate_entry(mContext, name, 1);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorFileNotExist;
    }

    int32_t len = mz_zip_reader_entry_save_buffer_length(mContext);
    if (len <= 0) {
        mZipErr = len;
        return ErrorZipEntryFailed;
    }

    int outErr = mOutput.ensure_buffer(mOutput.write_pos() + len);
    if (outErr != Success) {
        return outErr;
    }
    PureCore::DataRef freeBuffer = mOutput.free_buffer();
    err = mz_zip_reader_entry_save_buffer(mContext, freeBuffer.data(), len);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    mOutput.write_pos(mOutput.write_pos() + len);
    return Success;
}

int PureZipFileDecoder::finish_decode() {
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureZipFileDecoder::get_output() { return mOutput; }

void PureZipFileDecoder::clear() {
    clear_hold_output();
    mOutput.clear();
    mZipErr = MZ_OK;
}

void PureZipFileDecoder::clear_hold_output() {
    if (mContext != nullptr) {
        if (mz_zip_reader_is_open(mContext) == MZ_OK) {
            mz_zip_reader_close(mContext);
        }
        mz_zip_reader_delete(&mContext);
    }
    mEntries.clear();
}

bool PureZipFileDecoder::is_running() const { return mContext != nullptr; }

/////////////////////////////////////////////////////////////////
/// PureZipMemEncoder
///////////////////////////////////////////////////////////////
PureZipMemEncoder::PureZipMemEncoder()
    : mContext(nullptr), mOutput(nullptr), mOutputGrow(128 * 1024), mLevel(MZ_COMPRESS_LEVEL_NORMAL), mModifyDate(0), mPass(), mZipErr(MZ_OK) {}

PureZipMemEncoder::~PureZipMemEncoder() { clear(); }

int PureZipMemEncoder::set_encode_level(int16_t level) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mLevel = level;
    return Success;
}

int16_t PureZipMemEncoder::get_encode_level() const { return mLevel; }

int PureZipMemEncoder::set_modify_date(time_t date) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mModifyDate = date;
    return Success;
}

time_t PureZipMemEncoder::get_modify_date() const { return mModifyDate; }

int PureZipMemEncoder::set_password(const char* pass) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mPass = pass;
    return Success;
}

const std::string& PureZipMemEncoder::get_password() const { return mPass; }

int PureZipMemEncoder::set_output_grow(uint32_t grow) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mOutputGrow = grow;
    return Success;
}

uint32_t PureZipMemEncoder::get_output_grow() const { return mOutputGrow; }

int32_t PureZipMemEncoder::get_zip_error() const { return mZipErr; }

int PureZipMemEncoder::encode(const char* dirPath, bool recursive) {
    int err = start_encode();
    if (err != Success) {
        return err;
    }

    err = update_encode_entry_dir(dirPath, dirPath, recursive);
    if (err != Success) {
        return err;
    }
    return finish_encode();
}

int PureZipMemEncoder::start_encode() {
    clear();
    mContext = mz_zip_writer_create();
    if (mContext == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return ErrorZipOpenFailed;
    }
    mz_zip_writer_set_compress_method(mContext, MZ_COMPRESS_METHOD_DEFLATE);
    if (!mPass.empty()) {
#if defined(PURE_OPENSSL) && PURE_OPENSSL
        mz_zip_writer_set_password(mContext, mPass.c_str());
        mz_zip_writer_set_aes(mContext, 1);
#else
        return ErrorNotSupport;
#endif
    }
    mOutput = mz_stream_mem_create();
    if (mOutput == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return ErrorZipOpenFailed;
    }
    mz_stream_mem_set_grow_size(mOutput, mOutputGrow);
    int32_t err = mz_stream_mem_open(mOutput, nullptr, MZ_OPEN_MODE_CREATE);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }
    err = mz_zip_writer_open(mContext, mOutput, 0);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }
    return Success;
}

int PureZipMemEncoder::update_encode_entry_file(const char* name, const char* filePath) {
    if (mContext == nullptr) {
        mZipErr = MZ_PARAM_ERROR;
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_writer_add_file(mContext, filePath, name);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipMemEncoder::update_encode_entry_dir(const char* rootPath, const char* dirPath, bool recursive) {
    if (mContext == nullptr) {
        mZipErr = MZ_PARAM_ERROR;
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_writer_add_path(mContext, dirPath, rootPath, 0, recursive ? 1 : 0);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipMemEncoder::update_encode_entry(const char* name, PureCore::DataRef data) {
    if (mContext == nullptr) {
        mZipErr = MZ_PARAM_ERROR;
        return ErrorZipContextError;
    }
    mz_zip_file fileInfo = {0};
    fileInfo.filename = name;
    fileInfo.creation_date = mModifyDate;
    fileInfo.modified_date = mModifyDate;
    fileInfo.accessed_date = mModifyDate;
    fileInfo.version_madeby = MZ_VERSION_MADEBY;
    fileInfo.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    fileInfo.uncompressed_size = data.size();
    fileInfo.flag = MZ_ZIP_FLAG_UTF8;
#if defined(PURE_OPENSSL) && PURE_OPENSSL
    fileInfo.aes_version = MZ_AES_VERSION;
#endif
    int32_t err = mz_zip_writer_add_buffer(mContext, data.data(), (int32_t)data.size(), &fileInfo);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipMemEncoder::finish_encode() {
    clear_hold_output();
    return Success;
}

PureCore::DataRef PureZipMemEncoder::get_output() {
    const void* buf = nullptr;
    int32_t bufLen = 0;
    mz_stream_mem_get_buffer_length(mOutput, &bufLen);
    if (bufLen <= 0) {
        return PureCore::DataRef();
    }
    int32_t zErr = mz_stream_mem_get_buffer(mOutput, &buf);
    if (zErr != MZ_OK) {
        mZipErr = zErr;
        return PureCore::DataRef();
    }
    return PureCore::DataRef((const char*)buf, (size_t)bufLen);
}

void PureZipMemEncoder::clear() {
    clear_hold_output();
    if (mOutput != nullptr) {
        if (mz_stream_mem_is_open(mOutput) == MZ_OK) {
            mz_stream_mem_close(mOutput);
        }
        mz_stream_mem_delete(&mOutput);
    }
    mZipErr = MZ_OK;
}

void PureZipMemEncoder::clear_hold_output() {
    if (mContext != nullptr) {
        if (mz_zip_writer_is_open(mContext) == MZ_OK) {
            mz_zip_writer_close(mContext);
        }
        mz_zip_writer_delete(&mContext);
    }
}

bool PureZipMemEncoder::is_running() const { return mContext != nullptr; }

/////////////////////////////////////////////////////////////////
/// PureZipMemDecoder
///////////////////////////////////////////////////////////////
PureZipMemDecoder::PureZipMemDecoder() : mContext(nullptr), mCache(nullptr), mEntries(), mPass(), mZipErr(MZ_OK), mOutput() {}

PureZipMemDecoder::~PureZipMemDecoder() { clear(); }

int PureZipMemDecoder::set_password(const char* pass) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mPass = pass;
    return Success;
}

const std::string& PureZipMemDecoder::get_password() const { return mPass; }

int32_t PureZipMemDecoder::get_zip_error() const { return mZipErr; }

int PureZipMemDecoder::decode(PureCore::DataRef data, const char* dirPath) {
    if (!PureCore::dir_exist(dirPath)) {
        return ErrorPathNotExist;
    }
    int err = start_decode(data);
    if (err != Success) {
        return err;
    }
    size_t count = get_entries_count();
    if (count <= 0) {
        return ErrorZipEntryFailed;
    }

    std::string parentPath(dirPath);
    if (parentPath.empty()) {
        parentPath = "./";
    } else if (parentPath.back() != '/' || parentPath.back() != '\\') {
        parentPath.push_back('/');
    }
    for (size_t i = 0; i < count; ++i) {
        const char* entry = get_entry_name(i);
        if (entry == nullptr) {
            return ErrorZipEntryFailed;
        }
        std::string file(parentPath + entry);
        int err = update_decode_entry_file(entry, file.c_str());
        if (err != Success) {
            return err;
        }
    }
    return finish_decode();
}

int PureZipMemDecoder::start_decode(PureCore::DataRef data) {
    clear();
    mContext = mz_zip_reader_create();
    if (mContext == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return ErrorZipOpenFailed;
    }
    if (!mPass.empty()) {
        mz_zip_reader_set_password(mContext, mPass.c_str());
    }

    mCache = mz_stream_mem_create();
    if (mCache == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return ErrorZipOpenFailed;
    }
    mz_stream_mem_set_buffer(mCache, data.data(), (int32_t)data.size());
    int32_t err = mz_stream_mem_open(mCache, nullptr, MZ_OPEN_MODE_READ);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }

    err = mz_zip_reader_open(mContext, mCache);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }

    err = mz_zip_reader_goto_first_entry(mContext);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipOpenFailed;
    }

    mEntries.clear();
    do {
        mz_zip_file* fileInfo = nullptr;
        err = mz_zip_reader_entry_get_info(mContext, &fileInfo);
        if (err != MZ_OK) {
            mZipErr = err;
            return ErrorZipOpenFailed;
        }
        if (fileInfo != nullptr && fileInfo->filename != nullptr) {
            mEntries.push_back(fileInfo->filename);
        }
    } while (mz_zip_reader_goto_next_entry(mContext) == MZ_OK);
    return Success;
}

size_t PureZipMemDecoder::get_entries_count() {
    if (mContext == nullptr) {
        mZipErr = MZ_OPEN_ERROR;
        return 0;
    }
    if (mz_zip_reader_is_open(mContext) != MZ_OK) {
        mZipErr = MZ_OPEN_ERROR;
        return 0;
    }
    return int32_t(mEntries.size());
}

const char* PureZipMemDecoder::get_entry_name(size_t idx) const {
    if (mContext == nullptr || idx >= mEntries.size()) {
        return nullptr;
    }
    return mEntries[idx].c_str();
}

int PureZipMemDecoder::update_decode_entry_file(const char* name, const char* filePath) {
    if (mContext == nullptr) {
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_reader_locate_entry(mContext, name, 1);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorFileNotExist;
    }

    err = mz_zip_reader_entry_save_file(mContext, filePath);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    return Success;
}

int PureZipMemDecoder::update_decode_entry(const char* name) {
    if (mContext == nullptr) {
        return ErrorZipContextError;
    }

    int32_t err = mz_zip_reader_locate_entry(mContext, name, 1);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }

    int32_t len = mz_zip_reader_entry_save_buffer_length(mContext);
    if (len <= 0) {
        mZipErr = len;
        return ErrorZipEntryFailed;
    }

    int outErr = mOutput.ensure_buffer(mOutput.write_pos() + len);
    if (outErr != Success) {
        return outErr;
    }
    PureCore::DataRef freeBuffer = mOutput.free_buffer();
    err = mz_zip_reader_entry_save_buffer(mContext, freeBuffer.data(), len);
    if (err != MZ_OK) {
        mZipErr = err;
        return ErrorZipEntryFailed;
    }
    mOutput.write_pos(mOutput.write_pos() + len);
    return Success;
}

int PureZipMemDecoder::finish_decode() {
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureZipMemDecoder::get_output() { return mOutput; }

void PureZipMemDecoder::clear() {
    clear_hold_output();
    mOutput.clear();
    mZipErr = MZ_OK;
}

void PureZipMemDecoder::clear_hold_output() {
    if (mContext != nullptr) {
        if (mz_zip_reader_is_open(mContext) == MZ_OK) {
            mz_zip_reader_close(mContext);
        }
        mz_zip_reader_delete(&mContext);
    }
    if (mCache != nullptr) {
        if (mz_stream_mem_is_open(mCache) == MZ_OK) {
            mz_stream_mem_close(mCache);
        }
        mz_stream_mem_delete(&mCache);
    }
    mEntries.clear();
}

bool PureZipMemDecoder::is_running() const { return mContext != nullptr; }

/////////////////////////////////////////////////////////////////
/// PureZipStream
///////////////////////////////////////////////////////////////
static const char sZipBlockHeader[2]{'z', char(205)};
static const size_t sZipBlockHeaderSize = sizeof(sZipBlockHeader) + sizeof(uint32_t);
static const size_t sZipDefaultBlockSize = 8 * 1024;
static const size_t sZipMaxBlockSize = 1024 * 1024 * 4;

enum EPureZipDataDecodeStep {
    DecodeStepHeader = 0,
    DecodeStepLen = 1,
    DecodeStepData = 2,
};

/////////////////////////////////////////////////////////////////
/// PureZipStreamEncoder
///////////////////////////////////////////////////////////////
PureZipStreamEncoder::PureZipStreamEncoder()
    : mContext(nullptr), mBlockSize(sZipDefaultBlockSize), mLevel(MZ_COMPRESS_LEVEL_DEFAULT), mZipErr(MZ_OK), mInputCache(), mOutput() {}

PureZipStreamEncoder::~PureZipStreamEncoder() { clear(); }

int PureZipStreamEncoder::set_block_size(uint32_t size) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mBlockSize = size;
    return Success;
}

uint32_t PureZipStreamEncoder::get_block_size() const { return mBlockSize; }

int PureZipStreamEncoder::set_encode_level(int16_t level) {
    if (is_running()) {
        return ErrorZipStepError;
    }
    mLevel = level;
    return Success;
}

int16_t PureZipStreamEncoder::get_encode_level() const { return mLevel; }

int32_t PureZipStreamEncoder::get_zip_error() const { return mZipErr; }

int PureZipStreamEncoder::encode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorZipDataError;
    }
    int err = start_encode();
    if (err != Success) {
        return err;
    }
    err = update_encode(src);
    if (err != Success) {
        return err;
    }
    return finish_encode();
}

int PureZipStreamEncoder::start_encode() {
    if (mBlockSize == 0 || mBlockSize > sZipMaxBlockSize) {
        return ErrorZipBlockSizeError;
    }
    clear();
    mContext = new zng_stream{};
    int32_t zErr = zng_deflateInit(mContext, mLevel);
    if (zErr != Z_OK) {
        mZipErr = zErr;
        return ErrorZipEncodeFailed;
    }
    int err = mInputCache.resize_buffer(mBlockSize);
    if (err != Success) {
        return err;
    }
    err = mOutput.ensure_buffer(sZipBlockHeaderSize);
    if (err != Success) {
        return err;
    }
    err = mOutput.write(PureCore::DataRef(sZipBlockHeader, sizeof(sZipBlockHeader)));
    if (err != Success) {
        return err;
    }
    PureCore::u32_to_four_char(mBlockSize, mOutput.free_buffer().data());
    mOutput.write_pos(mOutput.write_pos() + sizeof(mBlockSize));
    return Success;
}
int PureZipStreamEncoder::update_encode(PureCore::DataRef src) {
    if (mContext == nullptr) {
        return ErrorZipStepError;
    }
    if (src.empty()) {
        return ErrorZipDataError;
    }

    while (mInputCache.size() >= mBlockSize) {
        int err = encode_data(PureCore::DataRef(mInputCache.data().data(), mBlockSize), false);
        if (err != Success) {
            return err;
        }
        mInputCache.read_pos(mInputCache.read_pos() + mBlockSize);
    }

    size_t idx = 0;
    size_t needSize = mBlockSize - mInputCache.size();
    while (idx + needSize < src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, needSize));
        if (err != Success) {
            return err;
        }
        err = encode_data(mInputCache.data(), false);
        if (err != Success) {
            return err;
        }
        mInputCache.clear();
        idx += needSize;
        needSize = mBlockSize - mInputCache.size();
    }

    if (idx < src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, src.size() - idx));
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int PureZipStreamEncoder::flush_encode() {
    if (mInputCache.size() > 0) {
        return encode_data(mInputCache.data(), false);
    }
    return Success;
}

int PureZipStreamEncoder::finish_encode() {
    if (mContext == nullptr) {
        return ErrorZipStepError;
    }
    int err = encode_data(mInputCache.data(), true);
    if (err != Success) {
        return err;
    }
    zng_deflateEnd(mContext);
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureZipStreamEncoder::get_output() { return mOutput; }

void PureZipStreamEncoder::clear() {
    clear_hold_output();
    mOutput.clear();
    mZipErr = MZ_OK;
}

int PureZipStreamEncoder::encode_data(PureCore::DataRef src, bool isEnd) {
    if (src.empty()) {
        return ErrorZipDataError;
    }
    size_t boundSize = zng_deflateBound(mContext, (unsigned long)src.size());
    int err = mOutput.ensure_buffer(mOutput.write_pos() + boundSize + sizeof(uint32_t));
    if (err != Success) {
        return err;
    }
    auto destBuffer = mOutput.free_buffer();
    mContext->next_in = (const unsigned char*)src.data();
    mContext->avail_in = (uint32_t)src.size();
    mContext->next_out = (unsigned char*)destBuffer.data() + sizeof(uint32_t);
    mContext->avail_out = (unsigned int)destBuffer.size() - sizeof(uint32_t);
    int32_t zErr = MZ_OK;
    do {
        zErr = zng_deflate(mContext, isEnd ? Z_FINISH : Z_SYNC_FLUSH);
        if (zErr == Z_STREAM_END) {
            break;
        }
        if (zErr != MZ_OK) {
            mZipErr = zErr;
            return ErrorZipDataError;
        }
    } while (mContext->avail_in > 0 || (isEnd && zErr == MZ_OK));

    uint32_t realSize = (uint32_t)destBuffer.size() - sizeof(uint32_t) - mContext->avail_out;
    PureCore::u32_to_four_char(realSize, destBuffer.data());
    mOutput.write_pos(mOutput.write_pos() + sizeof(uint32_t) + realSize);
    return Success;
}

void PureZipStreamEncoder::clear_hold_output() {
    if (mContext != nullptr) {
        mZipErr = zng_deflateEnd(mContext);
        delete mContext;
        mContext = nullptr;
    }
    mInputCache.clear();
}

bool PureZipStreamEncoder::is_running() const { return mContext != nullptr; }

/////////////////////////////////////////////////////////////////
/// PureZipStreamDecoder
///////////////////////////////////////////////////////////////
PureZipStreamDecoder::PureZipStreamDecoder()
    : mContext(nullptr), mBlockSize(0), mNeedSize(0), mDecodeStep(DecodeStepHeader), mZipErr(MZ_OK), mInputCache(), mOutput() {}

PureZipStreamDecoder::~PureZipStreamDecoder() { clear(); }

int32_t PureZipStreamDecoder::get_zip_error() const { return mZipErr; }

int PureZipStreamDecoder::decode(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorZipDataError;
    }
    int err = start_decode();
    if (err != Success) {
        return err;
    }
    err = update_decode(src);
    if (err != Success) {
        return err;
    }
    return finish_decode();
}

int PureZipStreamDecoder::start_decode() {
    clear();
    mContext = new zng_stream{};
    mNeedSize = sZipBlockHeaderSize;
    mDecodeStep = DecodeStepHeader;
    int32_t zErr = zng_inflateInit(mContext);
    if (zErr != Z_OK) {
        mZipErr = zErr;
        return ErrorZipEncodeFailed;
    }
    return mInputCache.resize_buffer(sZipBlockHeaderSize);
}

int PureZipStreamDecoder::update_decode(PureCore::DataRef src) {
    if (mContext == nullptr) {
        return ErrorZipStepError;
    }
    if (src.empty()) {
        return ErrorZipDataError;
    }

    while (mInputCache.size() >= mNeedSize) {
        PureCore::DataRef data(mInputCache.data().data(), mNeedSize);
        int err = decode_data(data);
        if (err != Success) {
            return err;
        }
        mInputCache.read_pos(mInputCache.read_pos() + data.size());
    }

    size_t idx = 0;
    size_t needSize = mNeedSize - mInputCache.size();
    while (idx + needSize <= src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, needSize));
        if (err != Success) {
            return err;
        }
        err = decode_data(mInputCache.data());
        if (err != Success) {
            return err;
        }
        mInputCache.clear();
        idx += needSize;
        needSize = mNeedSize - mInputCache.size();
    }

    if (idx < src.size()) {
        int err = mInputCache.write(PureCore::DataRef(src.data() + idx, src.size() - idx));
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int PureZipStreamDecoder::finish_decode() {
    if (mContext == nullptr || mDecodeStep != DecodeStepLen) {
        return ErrorZipStepError;
    }
    mZipErr = zng_inflateEnd(mContext);
    clear_hold_output();
    return Success;
}

PureCore::DynamicBuffer& PureZipStreamDecoder::get_output() { return mOutput; }

void PureZipStreamDecoder::clear() {
    clear_hold_output();
    mOutput.clear();
    mZipErr = MZ_OK;
}

int PureZipStreamDecoder::decode_data(PureCore::DataRef src) {
    if (src.empty()) {
        return ErrorZipDataError;
    }
    switch (mDecodeStep) {
        case DecodeStepHeader: {
            if (src.size() != sZipBlockHeaderSize) {
                return ErrorZipDataError;
            }
            if (memcmp(sZipBlockHeader, src.data(), sizeof(sZipBlockHeader)) != 0) {
                return ErrorZipDataError;
            }
            uint32_t blockSize = PureCore::four_char_to_u32(src.data() + sizeof(sZipBlockHeader));
            if (blockSize == 0 || blockSize > sZipMaxBlockSize) {
                return ErrorZipBlockSizeError;
            }
            mBlockSize = blockSize;
            mNeedSize = sizeof(uint32_t);
            mDecodeStep = DecodeStepLen;
            size_t boundSize = zng_deflateBound(mContext, mBlockSize);
            int err = mInputCache.resize_buffer(boundSize);
            if (err != Success) {
                return err;
            }
        } break;
        case DecodeStepLen: {
            if (src.size() != sizeof(uint32_t)) {
                return ErrorZipDataError;
            }
            uint32_t dataSize = PureCore::four_char_to_u32(src.data());
            if (dataSize == 0) {
                return ErrorZipDataError;
            }
            mNeedSize = dataSize;
            mDecodeStep = DecodeStepData;
        } break;
        case DecodeStepData: {
            if (src.size() != mNeedSize) {
                return ErrorZipDataError;
            }
            int err = mOutput.ensure_buffer(mOutput.write_pos() + mBlockSize);
            if (err != Success) {
                return err;
            }
            auto destBuffer = mOutput.free_buffer();
            mContext->next_in = (const unsigned char*)src.data();
            mContext->avail_in = (uint32_t)src.size();
            mContext->next_out = (unsigned char*)destBuffer.data();
            mContext->avail_out = (unsigned int)destBuffer.size();
            int32_t zErr = MZ_OK;
            do {
                zErr = zng_inflate(mContext, Z_SYNC_FLUSH);
                if (zErr == Z_STREAM_END) {
                    break;
                }
                if (zErr != MZ_OK) {
                    mZipErr = zErr;
                    return ErrorZipDataError;
                }
            } while (mContext->avail_in > 0);
            uint32_t realSize = (uint32_t)destBuffer.size() - mContext->avail_out;
            mOutput.write_pos(mOutput.write_pos() + realSize);
            mNeedSize = sizeof(uint32_t);
            mDecodeStep = DecodeStepLen;
        } break;
        default:
            return ErrorZipStepError;
    }
    return Success;
}

void PureZipStreamDecoder::clear_hold_output() {
    if (mContext != nullptr) {
        zng_inflateEnd(mContext);
        delete mContext;
        mContext = nullptr;
    }
    mBlockSize = 0;
    mNeedSize = 0;
    mDecodeStep = DecodeStepHeader;
    mInputCache.clear();
}

}  // namespace PureEncrypt
