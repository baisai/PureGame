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
#include "PureCore/OsHelper.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureEncrypt/PureZip.h"
#include "PureEncrypt/PureLz4.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureMsg/MsgErrorDesc.h"
#include "PureMsg/MsgBuffer.h"
#include "PureMsg/MsgArgs.h"
#include "PureDb/DbErrorDesc.h"
#include "PureDb/Leveldb/LevelConnector.h"

#include "leveldb/cache.h"
#include "leveldb/filter_policy.h"
#include "leveldb/write_batch.h"

#include <fstream>

namespace PureDb {
/////////////////////////////////////////////////////////////////
/// LevelLogger
///////////////////////////////////////////////////////////////
LevelLogger::LevelLogger() {
    int err = mBuffer.ensure_buffer(512);
    if (err != PureCore::Success) {
        PureError("LevelLogger alloc buffer failed {}", get_error_desc(err));
        return;
    }
    err = mBuffer.write(PureCore::StringRef("[leveldb]:"));
    if (err != PureCore::Success) {
        PureError("LevelLogger write buffer failed {}", get_error_desc(err));
        return;
    }
}

void LevelLogger::Logv(const char* format, va_list ap) {
    if (mBuffer.size() != 10) {
        PureError("LevelLogger log failed, buffer invalid");
        return;
    }
    auto buffer = mBuffer.free_buffer();
    int len = std::vsnprintf(buffer.data(), buffer.size(), format, ap);
    if (len < 0) {
        PureError("LevelLogger log failed, format failed");
        return;
    }
    mBuffer.write_pos(10 + len);
    PureInfo("{}", mBuffer.data());
    mBuffer.write_pos(10);
}

/////////////////////////////////////////////////////////////////
/// key tools
///////////////////////////////////////////////////////////////
enum ELevelDataType : int8_t {
    ELevelNone = 0,

    ELevelValue = 1,
    ELevelHash = 2,
    ELevelList = 3,

    ELevelMax = 4,
};

static void calc_one_key(ELevelDataType type, PureCore::StringRef key, std::string& out) {
    out.clear();
    out.push_back(type);
    out.append(key.data(), key.size());
}

static int64_t calc_one_key_offset() { return sizeof(ELevelDataType); }

static void calc_one_start_key(ELevelDataType type, std::string& out) {
    out.clear();
    out.push_back(type);
}

static void calc_one_end_key(ELevelDataType type, std::string& out) {
    out.clear();
    out.push_back(type + 1);
}

static void calc_two_key(ELevelDataType type, PureCore::StringRef name, PureCore::StringRef key, std::string& out) {
    out.clear();
    out.push_back(type);
    out.append(name.data(), name.size());
    out.push_back(0);
    out.append(key.data(), key.size());
}

static int64_t calc_two_key_offset(PureCore::StringRef name) { return sizeof(ELevelDataType) + name.size() + 1; }

static void calc_two_start_key(ELevelDataType type, PureCore::StringRef name, std::string& out) {
    out.clear();
    out.push_back(type);
    out.append(name.data(), name.size());
    out.push_back(0);
}

static void calc_two_end_key(ELevelDataType type, PureCore::StringRef name, std::string& out) {
    out.clear();
    out.push_back(type);
    out.append(name.data(), name.size());
    out.push_back(1);
}

static void calc_list_first_key(bool forward, std::string& out) {
    static const uint8_t left[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7f};
    static const uint8_t right[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81};
    out.clear();
    int err = Success;
    if (forward) {  // right
        out.append((const char*)right, sizeof(right));
    } else {  // left
        out.append((const char*)left, sizeof(left));
    }
}

static int calc_list_key(PureCore::StringRef key, bool forward, std::string& out) {
    if (key.size() != sizeof(int64_t)) {
        return ErrorLevelKeyInvalid;
    }
    bool keyForward = true;
    bool invalid = true;
    for (auto c : key) {
        uint8_t real = c;
        if (real > 0x80) {
            keyForward = true;
            invalid = false;
            break;
        } else if (real < 0x80) {
            keyForward = false;
            invalid = false;
            break;
        }
    }
    if (invalid) {
        return ErrorLevelKeyInvalid;
    }

    if (keyForward != forward) {
        calc_list_first_key(forward, out);
        return Success;
    }

    out.clear();
    out.append(key.data(), key.size());
    for (int32_t i = int32_t(out.size() - 1); i >= 0; --i) {
        int c = uint8_t(key[i]);
        if (keyForward) {
            ++c;
            if (c >= UINT8_MAX) {
                out[i] = char(0x80);
            } else {
                out[i] = char(c);
                break;
            }
        } else {
            --c;
            if (c <= 0) {
                out[i] = char(0x80);
            } else {
                out[i] = char(c);
                break;
            }
        }
        if (i == 0) {
            invalid = true;
        }
    }
    if (invalid) {
        return ErrorLevelKeyInvalid;
    }
    return Success;
}

// only use for range key
static int calc_list_next_key(PureCore::StringRef name, PureCore::StringRef key, bool forward, std::string& out) {
    int64_t offset = calc_two_key_offset(name);
    if (int64_t(key.size()) - offset != sizeof(int64_t)) {
        return ErrorLevelKeyInvalid;
    }
    out.clear();
    out.append(key.data(), key.size());

    int err = Success;
    for (int32_t i = int32_t(out.size() - 1); i >= offset; --i) {
        int c = uint8_t(key[i]);
        if (forward) {
            ++c;
            if (c >= UINT8_MAX) {
                out[i] = 0;
            } else {
                out[i] = char(c);
                break;
            }
        } else {
            --c;
            if (c <= 0) {
                out[i] = 0;
            } else {
                out[i] = char(c);
                break;
            }
        }
        if (i == 0) {
            err = ErrorLevelKeyInvalid;
        }
    }
    return err;
}

static void calc_all_start_key(std::string& out) {
    out.clear();
    out.push_back(ELevelNone);
}

static void calc_all_end_key(std::string& out) {
    out.clear();
    out.push_back(ELevelMax);
}

/////////////////////////////////////////////////////////////////
/// LevelConnector
///////////////////////////////////////////////////////////////

LevelConnector::~LevelConnector() { close(); }

int LevelConnector::connect(const LevelConfig& cfg) {
    if (mContext != nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    mOptions.reuse_logs = true;
    if (cfg.mEnableLog) {
        mOptions.info_log = new LevelLogger;
    }
    mOptions.create_if_missing = cfg.mCreateIfMiss;
    mOptions.error_if_exists = cfg.mErrorIfExists;
    mOptions.paranoid_checks = cfg.mParanoidCheck;
    mOptions.block_cache = leveldb::NewLRUCache(cfg.mCacheSize);
    mOptions.filter_policy = leveldb::NewBloomFilterPolicy(cfg.mBloomFilter);
    mOptions.write_buffer_size = cfg.mWriteBufferSize;
    mOptions.max_open_files = cfg.mMaxOpenFiles;
    mOptions.block_size = cfg.mBlockSize;
    mOptions.max_file_size = cfg.mMaxFileSize;

    mReadOption.fill_cache = cfg.mReadFillCache;
    mReadOption.verify_checksums = cfg.mReadChecksums;

    mWriteOption.sync = cfg.mWriteSync;

    mIterKeepTime = cfg.mIterKeepTime;

    auto sta = leveldb::DB::Open(mOptions, cfg.mPath, &mContext);
    if (!sta.ok() || mContext == nullptr) {
        close();
        use_reply()->set_status(sta);
        return ErrorLevelOpenFailed;
    }
    return Success;
}

void LevelConnector::close() {
    clear_iterator();
    cancel_backup();
    if (mContext != nullptr) {
        delete mContext;
        mContext = nullptr;
    }
    if (mOptions.info_log != nullptr) {
        delete mOptions.info_log;
        mOptions.info_log = nullptr;
    }
    if (mOptions.block_cache != nullptr) {
        delete mOptions.block_cache;
        mOptions.block_cache = nullptr;
    }
    if (mOptions.filter_policy != nullptr) {
        delete mOptions.filter_policy;
        mOptions.filter_policy = nullptr;
    }
    mReply.remove();
}

void LevelConnector::update() {
    gc_iterator();
    tick_backup();
}

int LevelConnector::vset(PureCore::StringRef name, PureCore::StringRef data) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty() || data.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    calc_one_key(ELevelValue, name, mKeyHelper1);
    return level_put(mKeyHelper1, data);
}

int LevelConnector::vdel(PureCore::StringRef name) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    calc_one_key(ELevelValue, name, mKeyHelper1);
    return level_del(mKeyHelper1);
}

int LevelConnector::vget(PureCore::StringRef name) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    calc_one_key(ELevelValue, name, mKeyHelper1);
    return level_get(mKeyHelper1);
}

int LevelConnector::vmset(PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (datas.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    std::vector<std::string> keys;
    std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> newDatas;
    keys.reserve(datas.size());
    newDatas.reserve(datas.size());
    for (auto& data : datas) {
        keys.emplace_back();
        calc_one_key(ELevelValue, data.first, keys.back());
        newDatas.push_back(std::make_pair(keys.back(), data.second));
    }

    int err = level_write(newDatas, PureCore::ArrayRef<PureCore::StringRef>());
    if (err != Success) {
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::vmdel(PureCore::ArrayRef<PureCore::StringRef> names) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (names.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    std::vector<std::string> keys;
    std::vector<PureCore::StringRef> newKeys;
    keys.reserve(names.size());
    newKeys.reserve(names.size());
    for (auto& name : names) {
        keys.emplace_back();
        calc_one_key(ELevelValue, name, keys.back());
        newKeys.push_back(keys.back());
    }
    int err = level_write(PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>>(), newKeys);
    if (err != Success) {
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::vmget(PureCore::ArrayRef<PureCore::StringRef> names) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (names.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    LevelReply* reply = LevelReply::get();
    if (reply == nullptr) {
        return ErrorMemoryNotEnough;
    }
    for (auto& name : names) {
        calc_one_key(ELevelValue, name, mKeyHelper1);
        int err = level_get(mKeyHelper1);
        if (err != Success) {
            LevelReply* sub = LevelReply::get();
            if (sub == nullptr) {
                LevelReply::free(reply);
                return ErrorMemoryNotEnough;
            }
            sub->set_error(err);
            reply->append_array(sub);
            continue;
        }
        reply->append_array(pop_reply());
    }
    reset_replay(reply);
    return Success;
}

size_t LevelConnector::v_approximate_size(PureCore::StringRef name) {
    if (mContext == nullptr) {
        return 0;
    }
    calc_one_key(ELevelValue, name, mKeyHelper1);
    leveldb::Range range(mKeyHelper1, mKeyHelper1);
    size_t s = 0;
    mContext->GetApproximateSizes(&range, 1, &s);
    return s;
}

int64_t LevelConnector::create_v_iterator(PureCore::StringRef start, PureCore::StringRef end, bool forward) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return 0;
    }
    if (start.empty()) {
        calc_one_start_key(ELevelValue, mKeyHelper1);
    } else {
        calc_one_key(ELevelValue, start, mKeyHelper1);
    }
    if (end.empty()) {
        calc_one_end_key(ELevelValue, mKeyHelper2);
    } else {
        calc_one_key(ELevelValue, end, mKeyHelper2);
    }
    int64_t id = 0;
    int64_t offset = calc_one_key_offset();
    id = create_range_iterator(offset, mKeyHelper1, mKeyHelper2, forward);
    return id;
}

int LevelConnector::hset(PureCore::StringRef name, PureCore::StringRef key, PureCore::StringRef data) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty() || key.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    calc_two_key(ELevelHash, name, key, mKeyHelper1);
    return level_put(mKeyHelper1, data);
}

int LevelConnector::hdel(PureCore::StringRef name, PureCore::StringRef key) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty() || key.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    calc_two_key(ELevelHash, name, key, mKeyHelper1);
    return level_del(mKeyHelper1);
}

int LevelConnector::hget(PureCore::StringRef name, PureCore::StringRef key) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty() || key.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    calc_two_key(ELevelHash, name, key, mKeyHelper1);
    return level_get(mKeyHelper1);
}

int LevelConnector::hmset(PureCore::StringRef name, PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty() || datas.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    std::vector<std::string> keys;
    std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> newDatas;
    keys.reserve(datas.size());
    newDatas.reserve(datas.size());
    for (auto& data : datas) {
        keys.emplace_back();
        calc_two_key(ELevelHash, name, data.first, keys.back());
        newDatas.push_back(std::make_pair(keys.back(), data.second));
    }

    int err = level_write(newDatas, PureCore::ArrayRef<PureCore::StringRef>());
    if (err != Success) {
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::hmdel(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty() || keys.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    std::vector<std::string> keysBuffer;
    std::vector<PureCore::StringRef> newKeys;
    keysBuffer.reserve(keys.size());
    newKeys.reserve(keys.size());
    for (auto& key : keys) {
        keysBuffer.emplace_back();
        calc_two_key(ELevelHash, name, key, keysBuffer.back());
        newKeys.push_back(keysBuffer.back());
    }

    int err = level_write(PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>>(), newKeys);
    if (err != Success) {
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::hmget(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (keys.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    LevelReply* reply = LevelReply::get();
    if (reply == nullptr) {
        return ErrorMemoryNotEnough;
    }
    for (auto& key : keys) {
        calc_two_key(ELevelHash, name, key, mKeyHelper1);
        int err = level_get(mKeyHelper1);
        if (err != Success) {
            LevelReply* sub = LevelReply::get();
            if (sub == nullptr) {
                LevelReply::free(reply);
                return ErrorMemoryNotEnough;
            }
            sub->set_error(err);
            reply->append_array(sub);
            continue;
        }
        reply->append_array(pop_reply());
    }
    reset_replay(reply);
    return Success;
}

int LevelConnector::hgetall(PureCore::StringRef name) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    calc_two_start_key(ELevelHash, name, mKeyHelper1);
    calc_two_end_key(ELevelHash, name, mKeyHelper2);

    int err = Success;
    auto reply = use_reply();
    int64_t offset = calc_two_key_offset(name);
    level_range(mKeyHelper1, mKeyHelper2, true, [&err, reply, offset](PureCore::StringRef key, PureCore::StringRef val) {
        LevelReply* sub = LevelReply::get();
        if (sub == nullptr) {
            err = ErrorMemoryNotEnough;
            return false;
        }
        reply->append_array(sub);
        if (offset >= 0 && key.size() >= size_t(offset)) {
            PureCore::StringRef realKey(key.data() + offset, key.size() - offset);
            sub->set_key(realKey);
        }
        sub->set_value(val);
        return true;
    });

    if (err != Success) {
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::hdrop(PureCore::StringRef name) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    calc_two_start_key(ELevelHash, name, mKeyHelper1);
    calc_two_end_key(ELevelHash, name, mKeyHelper2);

    int err = Success;
    int64_t offset = calc_two_key_offset(name);
    level_range(mKeyHelper1, mKeyHelper2, true, [&err, offset, this](PureCore::StringRef key, PureCore::StringRef val) {
        if (offset >= 0 && key.size() >= size_t(offset)) {
            PureCore::StringRef realKey(key.data() + offset, key.size() - offset);
            err = level_del(realKey);
            return err == Success;
        }
        return true;
    });
    return err;
}

size_t LevelConnector::h_approximate_size(PureCore::StringRef name) {
    if (mContext == nullptr) {
        return 0;
    }
    calc_two_start_key(ELevelHash, name, mKeyHelper1);
    calc_two_end_key(ELevelHash, name, mKeyHelper2);

    leveldb::Range range(mKeyHelper1, mKeyHelper2);
    size_t s = 0;
    mContext->GetApproximateSizes(&range, 1, &s);
    return s;
}

int64_t LevelConnector::create_h_iterator(PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end, bool forward) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return 0;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return 0;
    }

    if (start.empty()) {
        calc_two_start_key(ELevelHash, name, mKeyHelper1);
    } else {
        calc_two_key(ELevelHash, name, start, mKeyHelper1);
    }
    if (end.empty()) {
        calc_two_end_key(ELevelHash, name, mKeyHelper2);
    } else {
        calc_two_key(ELevelHash, name, end, mKeyHelper2);
    }

    int64_t id = 0;
    int64_t offset = calc_two_key_offset(name);
    id = create_range_iterator(offset, mKeyHelper1, mKeyHelper2, forward);
    return id;
}

int LevelConnector::sset(PureCore::StringRef name, PureCore::StringRef key) { return hset(name, key, PureCore::StringRef()); }

int LevelConnector::sdel(PureCore::StringRef name, PureCore::StringRef key) { return hdel(name, key); }

bool LevelConnector::sget(PureCore::StringRef name, PureCore::StringRef key) {
    bool r = hget(name, key) == Success;
    pop_reply();
    return r;
}

int LevelConnector::smset(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) {
    std::vector<std::pair<PureCore::StringRef, PureCore::StringRef>> datas;
    for (auto& key : keys) {
        datas.push_back(std::make_pair(key, PureCore::StringRef()));
    }
    return hmset(name, datas);
}

int LevelConnector::smdel(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys) { return hmdel(name, keys); }

int LevelConnector::sgetall(PureCore::StringRef name) { return hgetall(name); }

int LevelConnector::sdrop(PureCore::StringRef name) { return hdrop(name); }

size_t LevelConnector::s_approximate_size(PureCore::StringRef name) { return h_approximate_size(name); }

int64_t LevelConnector::create_s_iterator(PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end, bool forward) {
    return create_h_iterator(name, start, end, forward);
}

int LevelConnector::lset(PureCore::StringRef name, int64_t idx, PureCore::StringRef data) {
    int err = real_lget(name, idx);
    if (err != Success) {
        return err;
    }
    auto reply = pop_reply();
    if (reply && reply->has_key()) {
        err = level_put(reply->get_key(), data);
    } else {
        err = ErrorLevelNotFound;
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::llpush(PureCore::StringRef name, PureCore::StringRef data) { return real_lpush(name, data, true); }
int LevelConnector::llpop(PureCore::StringRef name) {
    int err = real_lpop(name, true);
    if (err != Success) {
        return err;
    }
    if (mReply) {
        mReply->clear_key();
    }
    return err;
}

int LevelConnector::lrpush(PureCore::StringRef name, PureCore::StringRef data) { return real_lpush(name, data, false); }
int LevelConnector::lrpop(PureCore::StringRef name) {
    int err = real_lpop(name, false);
    if (err != Success) {
        return err;
    }
    if (mReply) {
        mReply->clear_key();
    }
    return err;
}

int LevelConnector::lget(PureCore::StringRef name, int64_t idx) {
    int err = real_lget(name, idx);
    if (err != Success) {
        return err;
    }
    if (mReply) {
        mReply->clear_key();
    }
    return err;
}
int LevelConnector::lrange(PureCore::StringRef name, int64_t start, int64_t end) {
    LevelReplyPtr startReply;
    LevelReplyPtr endReply;
    int err = Success;
    do {
        err = real_lget(name, start);
        if (err != Success) {
            break;
        }
        startReply = pop_reply();
        if (!startReply || !startReply->has_key()) {
            err = ErrorLevelNotFound;
            break;
        }
        err = real_lget(name, end);
        if (err != Success) {
            break;
        }
        endReply = pop_reply();
        if (!endReply || !endReply->has_key()) {
            err = ErrorLevelNotFound;
            break;
        }
        err = calc_list_next_key(name, endReply->get_key(), true, mKeyHelper1);
        if (err != Success) {
            break;
        }
        auto reply = use_reply();
        level_range(startReply->get_key(), mKeyHelper1, true, [&err, this, reply](PureCore::StringRef key, PureCore::StringRef val) {
            LevelReply* sub = LevelReply::get();
            if (sub == nullptr) {
                err = ErrorMemoryNotEnough;
                return false;
            }
            reply->append_array(sub);
            sub->set_value(val);
            return true;
        });
    } while (false);
    if (err != Success) {
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::ldel(PureCore::StringRef name, int64_t count, PureCore::StringRef data) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty() || count <= 0) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    calc_two_start_key(ELevelList, name, mKeyHelper1);
    calc_two_end_key(ELevelList, name, mKeyHelper2);

    int64_t cur = count;
    int err = Success;
    level_range(mKeyHelper1, mKeyHelper2, true, [&cur, &data, &err, this, count](PureCore::StringRef key, PureCore::StringRef val) {
        if (cur <= 0) {
            return false;
        }
        leveldb::Slice value(val.data(), val.size());
        if (value.compare(leveldb::Slice(data.data(), data.size())) == 0) {  // found
            err = level_del(key);
            if (err != Success) {
                PureError("level del {} failed, already del count {}", key, count - cur);
                return false;
            }
            --cur;
        }
        return true;
    });
    if (err == Success) {
        use_reply()->set_int(count - cur);
    }
    return err;
}

int LevelConnector::ltrim(PureCore::StringRef name, int64_t start, int64_t end) {
    LevelReplyPtr startReply;
    LevelReplyPtr endReply;
    int err = Success;
    do {
        err = real_lget(name, start);
        if (err != Success) {
            break;
        }
        startReply = pop_reply();
        if (!startReply || !startReply->has_key()) {
            use_reply()->set_error(ErrorLevelNotFound);
            err = ErrorLevelNotFound;
            break;
        }
        err = real_lget(name, end);
        if (err != Success) {
            break;
        }
        endReply = pop_reply();
        if (!endReply || !endReply->has_key()) {
            use_reply()->set_error(ErrorLevelNotFound);
            err = ErrorLevelNotFound;
            break;
        }
        err = calc_list_next_key(name, endReply->get_key(), true, mKeyHelper1);
        if (err != Success) {
            use_reply()->set_error(err);
            break;
        }
        level_range(PureCore::StringRef(), startReply->get_key(), true, [&err, this](PureCore::StringRef key, PureCore::StringRef val) {
            err = level_del(key);
            if (err != Success) {
                return false;
            }
            return true;
        });
        if (err != Success) {
            break;
        }
        level_range(mKeyHelper1, PureCore::StringRef(), true, [&err, this](PureCore::StringRef key, PureCore::StringRef val) {
            err = level_del(key);
            if (err != Success) {
                return false;
            }
            return true;
        });

    } while (false);
    return err;
}

int LevelConnector::lgetall(PureCore::StringRef name) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    calc_two_start_key(ELevelList, name, mKeyHelper1);
    calc_two_end_key(ELevelList, name, mKeyHelper2);

    auto reply = use_reply();
    if (reply == nullptr) {
        return ErrorMemoryNotEnough;
    }

    int err = Success;
    level_range(mKeyHelper1, mKeyHelper2, true, [&err, reply](PureCore::StringRef key, PureCore::StringRef val) {
        LevelReply* sub = LevelReply::get();
        if (sub == nullptr) {
            err = ErrorMemoryNotEnough;
            return false;
        }
        sub->set_value(val);
        reply->append_array(sub);
        return true;
    });
    if (err != Success) {
        reply->set_error(err);
    }
    return err;
}

int LevelConnector::ldrop(PureCore::StringRef name) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }

    calc_two_start_key(ELevelList, name, mKeyHelper1);
    calc_two_end_key(ELevelList, name, mKeyHelper2);

    int err = Success;
    level_range(mKeyHelper1, mKeyHelper2, true, [&err, this](PureCore::StringRef key, PureCore::StringRef val) {
        err = level_del(key);
        return err == Success;
    });
    return err;
}

size_t LevelConnector::l_approximate_size(PureCore::StringRef name) {
    if (mContext == nullptr) {
        return 0;
    }

    calc_two_start_key(ELevelList, name, mKeyHelper1);
    calc_two_end_key(ELevelList, name, mKeyHelper2);

    leveldb::Range range(mKeyHelper1, mKeyHelper2);
    size_t s = 0;
    mContext->GetApproximateSizes(&range, 1, &s);
    return s;
}

int64_t LevelConnector::create_l_iterator(PureCore::StringRef name, bool forward) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return 0;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return 0;
    }

    if (forward) {
        calc_two_start_key(ELevelList, name, mKeyHelper1);
        calc_two_end_key(ELevelList, name, mKeyHelper2);
    } else {
        calc_two_end_key(ELevelList, name, mKeyHelper1);
        calc_two_start_key(ELevelList, name, mKeyHelper2);
    }

    int64_t id = create_range_iterator(-1, mKeyHelper1, mKeyHelper2, forward);
    return id;
}

size_t LevelConnector::approximate_size() {
    if (mContext == nullptr) {
        return 0;
    }

    calc_all_start_key(mKeyHelper1);
    calc_all_end_key(mKeyHelper2);

    leveldb::Range range(mKeyHelper1, mKeyHelper2);
    size_t s = 0;
    mContext->GetApproximateSizes(&range, 1, &s);
    return s;
}

int LevelConnector::property(PureCore::StringRef name) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }

    auto reply = use_reply();
    auto& value = reply->use_value();
    if (!mContext->GetProperty(leveldb::Slice(name.data(), name.size()), &value)) {
        reply->set_error(ErrorLevelCommandFailed);
        return ErrorLevelCommandFailed;
    }
    return Success;
}

void LevelConnector::compact() {
    if (mContext == nullptr) {
        return;
    }
    PureInfo("level start compact");
    int64_t now = PureCore::steady_milli_s();
    mContext->CompactRange(nullptr, nullptr);
    PureInfo("level finish compact cost {} S", (PureCore::steady_milli_s() - now) / 1000);
}

class LevelBackupHanler {
public:
    LevelBackupHanler() = default;
    friend class LevelConnector;
    virtual ~LevelBackupHanler() {
        if (mContext != nullptr) {
            if (mOption.snapshot != nullptr) {
                mContext->ReleaseSnapshot(mOption.snapshot);
                mOption.snapshot = nullptr;
            }
            if (mIter != nullptr) {
                delete mIter;
                mIter = nullptr;
            }
            mContext = nullptr;
        }
    }

protected:
    int config(leveldb::DB* db, const char* path, int64_t batch) {
        if (db == nullptr || path == nullptr) {
            return ErrorInvalidArg;
        }
        if (mContext != nullptr) {
            return ErrorLevelStateInvalid;
        }

        auto dir = PureCore::get_parent_path(path);
        PureCore::create_dir(dir.c_str());
        mFile.open(path, std::ios::binary | std::ios::ate | std::ios::app);
        if (!mFile.is_open()) {
            return ErrorLevelBackupFailed;
        }
        if (start_encode() != PureEncrypt::Success) {
            return ErrorLevelBackupFailed;
        }
        mContext = db;
        mOption.fill_cache = false;
        mOption.verify_checksums = true;
        mOption.snapshot = db->GetSnapshot();
        if (mOption.snapshot == nullptr) {
            return ErrorMemoryNotEnough;
        }
        mIter = db->NewIterator(mOption);
        if (mIter == nullptr) {
            return ErrorMemoryNotEnough;
        }
        mIter->SeekToFirst();
        mBatch = batch;
        mStartTime = PureCore::steady_milli_s();
        return Success;
    }

    int write_head(char head) {
        mFile.write(&head, 1);
        if (!mFile.good()) {
            return ErrorLevelBackupFailed;
        }
        return Success;
    }

    int backup() {
        if (mIter == nullptr) {
            return ErrorInvalidArg;
        }
        int64_t count = mBatch;
        while (mIter->Valid()) {
            auto key = mIter->key();
            auto val = mIter->value();
            mBuffer.clear();
            int err = PureMsg::pack_args(mBuffer, PureCore::StringRef(key.data(), key.size()), PureCore::StringRef(val.data(), val.size()));
            if (err != PureMsg::Success) {
                return ErrorLevelBackupFailed;
            }
            err = update_encode(mBuffer.data());
            if (err != PureEncrypt::Success) {
                return ErrorLevelBackupFailed;
            }

            auto data = get_output().data();
            mFile.write(data.data(), data.size());
            if (!mFile.good()) {
                return ErrorLevelBackupFailed;
            }
            get_output().clear();
            mIter->Next();
            --count;
            if (mBatch >= 0 && count <= 0) {
                break;
            }
        }
        if (!mIter->status().ok()) {
            return ErrorLevelBackupFailed;
        }
        if (is_finish()) {
            int err = finish_encode();
            if (err != PureEncrypt::Success) {
                return ErrorLevelBackupFailed;
            }
            auto data = get_output().data();
            mFile.write(data.data(), data.size());
            if (!mFile.good()) {
                return ErrorLevelBackupFailed;
            }
            get_output().clear();
            mFile.flush();
        }
        return Success;
    }

    bool is_finish() const { return !mIter->Valid(); }

    virtual int start_encode() = 0;
    virtual int update_encode(PureCore::DataRef src) = 0;
    virtual int finish_encode() = 0;
    virtual PureCore::DynamicBuffer& get_output() = 0;

    PureCore::DynamicBuffer mBuffer;
    std::ofstream mFile;
    int64_t mBatch = 0;
    leveldb::DB* mContext = nullptr;
    leveldb::ReadOptions mOption;
    leveldb::Iterator* mIter = nullptr;
    int64_t mStartTime = 0;
};
class LevelBackupHanlerLz4 : public LevelBackupHanler {
public:
    friend class LevelConnector;
    LevelBackupHanlerLz4() = default;
    virtual ~LevelBackupHanlerLz4() = default;

protected:
    virtual int start_encode() { return mEncoder.start_encode(); }
    virtual int update_encode(PureCore::DataRef src) { return mEncoder.update_encode(src); }
    virtual int finish_encode() { return mEncoder.finish_encode(); }
    virtual PureCore::DynamicBuffer& get_output() { return mEncoder.get_output(); }

    PureEncrypt::PureLz4BlockEncoder mEncoder;
};
class LevelBackupHanlerZip : public LevelBackupHanler {
public:
    friend class LevelConnector;
    LevelBackupHanlerZip() = default;
    virtual ~LevelBackupHanlerZip() = default;

protected:
    virtual int start_encode() { return mEncoder.start_encode(); }
    virtual int update_encode(PureCore::DataRef src) { return mEncoder.update_encode(src); }
    virtual int finish_encode() { return mEncoder.finish_encode(); }
    virtual PureCore::DynamicBuffer& get_output() { return mEncoder.get_output(); }

    PureEncrypt::PureZipStreamEncoder mEncoder;
};
int LevelConnector::backup_to(const char* path, int64_t batch, ELevelBackupCompress compress) {
    if (path == nullptr) {
        return ErrorInvalidArg;
    }
    if (mContext == nullptr || mBackupHandler != nullptr) {
        return ErrorLevelStateInvalid;
    }
    LevelBackupHanler* bk = nullptr;
    if (compress == ELevelCompressLz4) {
        bk = new LevelBackupHanlerLz4;
    } else if (compress == ELevelCompressZip) {
        bk = new LevelBackupHanlerZip;
    } else {
        return ErrorInvalidArg;
    }
    if (bk == nullptr) {
        return ErrorMemoryNotEnough;
    }
    int err = bk->config(mContext, path, batch);
    if (err != Success) {
        delete bk;
        return err;
    }
    err = bk->write_head((int8_t)compress);
    if (err != Success) {
        delete bk;
        return err;
    }
    PureInfo("Level start backup to {} batch {} compress {}", path, batch, int8_t(compress));
    mBackupHandler = bk;
    if (batch <= 0) {
        err = bk->backup();
        cancel_backup();
    }
    return err;
}

bool LevelConnector::is_backuping() const { return mBackupHandler != nullptr; }

class RevertHandler {
protected:
    friend class LevelConnector;
    virtual ~RevertHandler() {}

    int config(const char* path, int64_t batch, std::function<int(PureCore::StringRef, PureCore::StringRef)> putter) {
        if (path == nullptr || !putter) {
            return ErrorInvalidArg;
        }
        mFile.open(path, std::ios::binary | std::ios::in);
        if (!mFile.is_open()) {
            return ErrorLevelRevertFailed;
        }
        mFile.seekg(1);  // skip head
        if (!mFile.good()) {
            return ErrorLevelRevertFailed;
        }
        int err = start_decode();
        if (err != PureEncrypt::Success) {
            return ErrorLevelRevertFailed;
        }
        err = mBuffer.resize_buffer(1024 * 1024);
        if (err != PureCore::Success) {
            return ErrorLevelRevertFailed;
        }
        mBatch = batch;
        mPutter = putter;
        return Success;
    }

    int revert() {
        int64_t count = mBatch;
        while (!mFile.eof()) {
            mBuffer.clear();
            auto data = mBuffer.free_buffer();
            mFile.read(data.data(), data.size());
            mBuffer.write_pos(mBuffer.write_pos() + mFile.gcount());
            int err = update_decode(mBuffer.data());
            if (err != PureEncrypt::Success) {
                return ErrorLevelRevertFailed;
            }
            err = put(count);
            if (err != Success) {
                return err;
            }
            if (mBatch > 0 && count <= 0) {
                break;
            }
        }
        if (mFile.eof()) {
            int err = finish_decode();
            if (err != PureEncrypt::Success) {
                return ErrorLevelRevertFailed;
            }
            err = put(count);
            if (err != Success) {
                return err;
            }
        }
        return Success;
    }

    int put(int64_t& count) {
        PureCore::StringRef key, val;
        auto& out = get_output();
        while (out.size() > 0) {
            size_t pos = out.read_pos();
            int err = PureMsg::unpack_args(out, key, val);
            if (err == PureMsg::ErrorReadBufferFailed) {  // need more data
                out.read_pos(pos);
                break;
            } else if (err == PureMsg::Success) {
                err = mPutter(key, val);
                if (err != Success) {
                    return ErrorLevelRevertFailed;
                }
                --count;
            } else {
                return ErrorLevelRevertFailed;
            }
        }
        out.align_data();
        return Success;
    }

    virtual int start_decode() = 0;
    virtual int update_decode(PureCore::DataRef src) = 0;
    virtual int finish_decode() = 0;
    virtual PureCore::DynamicBuffer& get_output() = 0;

    std::ifstream mFile;
    PureCore::DynamicBuffer mBuffer;
    int64_t mBatch;
    std::function<int(PureCore::StringRef, PureCore::StringRef)> mPutter;
};
class RevertHandlerLz4 : public RevertHandler {
protected:
    friend class LevelConnector;
    virtual ~RevertHandlerLz4() {}
    virtual int start_decode() { return mDecoder.start_decode(); }
    virtual int update_decode(PureCore::DataRef src) { return mDecoder.update_decode(src); }
    virtual int finish_decode() { return mDecoder.finish_decode(); }
    virtual PureCore::DynamicBuffer& get_output() { return mDecoder.get_output(); }

    PureEncrypt::PureLz4BlockDecoder mDecoder;
};
class RevertHandlerZip : public RevertHandler {
protected:
    friend class LevelConnector;
    virtual ~RevertHandlerZip() {}
    virtual int start_decode() { return mDecoder.start_decode(); }
    virtual int update_decode(PureCore::DataRef src) { return mDecoder.update_decode(src); }
    virtual int finish_decode() { return mDecoder.finish_decode(); }
    virtual PureCore::DynamicBuffer& get_output() { return mDecoder.get_output(); }

    PureEncrypt::PureZipStreamDecoder mDecoder;
};
int LevelConnector::revert_from(const char* path) {
    if (path == nullptr) {
        return ErrorInvalidArg;
    }
    std::ifstream ifs;
    ifs.open(path, std::ios::binary | std::ios::in);
    if (!ifs.is_open()) {
        return ErrorLevelRevertFailed;
    }
    char compress = 0;
    ifs.get(compress);
    if (!ifs.good()) {
        return ErrorLevelRevertFailed;
    }
    ifs.close();
    RevertHandler* rv = nullptr;
    if (compress == ELevelCompressLz4) {
        rv = new RevertHandlerLz4;
    } else if (compress == ELevelCompressZip) {
        rv = new RevertHandlerZip;
    } else {
        return ErrorLevelRevertFailed;
    }
    if (rv == nullptr) {
        return ErrorMemoryNotEnough;
    }

    int err = rv->config(path, 0, [this](PureCore::StringRef key, PureCore::StringRef val) { return level_put(key, val); });
    if (err != Success) {
        delete rv;
        return err;
    }
    err = rv->revert();
    delete rv;
    return err;
}

class LevelIteratorKeeper : public PureCore::Node {
private:
    friend class LevelConnector;
    LevelIteratorKeeper(leveldb::DB* context) : mContext(context) {}
    virtual ~LevelIteratorKeeper() {
        if (mContext == nullptr) {
            PureError("LevelIteratorKeeper destroy failed, context is nullptr, maybe leak memory");
        } else if (mOption.snapshot != nullptr) {
            mContext->ReleaseSnapshot(mOption.snapshot);
            mOption.snapshot = nullptr;
        }
        if (mIter != nullptr) {
            delete mIter;
        }
    }

    bool check_error() {
        if (mContext == nullptr || mIter == nullptr) {
            return true;
        }
        return !mIter->status().ok();
    }

    bool check_end() {
        if (check_error()) {
            return true;
        }
        if (!mIter->Valid()) {
            return true;
        }
        if (!mEndKey.empty()) {
            if (mEndForward) {
                return mIter->key().compare(mEndKey) >= 0;
            } else {
                return mIter->key().compare(mEndKey) <= 0;
            }
        }
        return false;
    }

    int64_t mID = 0;
    leveldb::DB* mContext = nullptr;
    leveldb::ReadOptions mOption;
    leveldb::Iterator* mIter = nullptr;
    int64_t mKeepTime = 0;
    std::string mEndKey;
    bool mEndForward = true;  // end if greater than mEndKey by true
    int64_t mKeyOffset = 0;   // < 0 no key, == 0 raw key, > 0 offset key
};
int LevelConnector::iterator(int64_t id, size_t count, bool forward) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    gc_iterator();
    auto iter = mIterMap.find(id);
    if (iter == mIterMap.end()) {
        return false;
    }
    auto keeper = (LevelIteratorKeeper*)iter->second;
    if (keeper == nullptr) {
        use_reply()->set_error(ErrorLevelIteratorInvalid);
        return 0;
    }
    keeper->mKeepTime = PureCore::steady_milli_s();
    return read_iterator(keeper, count, forward);
}

bool LevelConnector::remove_iterator(int64_t id) {
    if (mContext == nullptr) {
        return false;
    }
    auto iter = mIterMap.find(id);
    if (iter == mIterMap.end()) {
        return false;
    }
    auto keeper = (LevelIteratorKeeper*)iter->second;
    mIterMap.erase(iter);
    if (keeper != nullptr) {
        delete keeper;
    }
    return true;
}

LevelReplyPtr LevelConnector::pop_reply() { return mReply; }

LevelReply* LevelConnector::get_reply() { return mReply.get(); }

PureCore::StringRef LevelConnector::get_last_error() const {
    if (!mReply || !mReply->is_error()) {
        return PureCore::StringRef();
    }
    return mReply->get_error();
}

int64_t LevelConnector::create_range_iterator(int64_t keyOffset, PureCore::StringRef start, PureCore::StringRef end, bool forward) {
    auto keeper = (LevelIteratorKeeper*)create_iterator();
    if (keeper == nullptr) {
        return 0;
    }
    if (start.empty()) {
        if (forward) {
            keeper->mIter->SeekToFirst();
        } else {
            keeper->mIter->SeekToLast();
        }
    } else {
        keeper->mIter->Seek(leveldb::Slice(start.data(), start.size()));
        if (!forward) {
            if (keeper->mIter->Valid()) {
                keeper->mIter->Prev();
            } else {
                keeper->mIter->SeekToLast();
            }
        }
    }
    keeper->mEndKey.assign(end.data(), end.size());
    keeper->mEndForward = forward;
    keeper->mKeyOffset = keyOffset;
    return keeper->mID;
}

void* LevelConnector::create_iterator() {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return nullptr;
    }
    gc_iterator();
    LevelIteratorKeeper* keeper = new LevelIteratorKeeper(mContext);
    if (keeper == nullptr) {
        use_reply()->set_error(ErrorMemoryNotEnough);
        return nullptr;
    }
    keeper->mOption.fill_cache = false;
    keeper->mOption.verify_checksums = mReadOption.verify_checksums;
    keeper->mOption.snapshot = mContext->GetSnapshot();
    keeper->mIter = mContext->NewIterator(keeper->mOption);
    if (keeper->mIter == nullptr) {
        delete keeper;
        use_reply()->set_error(ErrorMemoryNotEnough);
        return 0;
    }
    keeper->mID = mIterID.gen_id();
    keeper->mKeepTime = PureCore::steady_milli_s();
    mIterList.push_back(keeper);
    mIterMap.insert(std::make_pair(keeper->mID, keeper));
    return keeper;
}

int LevelConnector::read_iterator(void* handler, size_t count, bool forward) {
    auto keeper = (LevelIteratorKeeper*)handler;
    if (keeper == nullptr || count == 0) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    auto iter = keeper->mIter;
    if (iter == nullptr) {
        use_reply()->set_error(ErrorLevelIteratorInvalid);
        return ErrorLevelIteratorInvalid;
    }
    auto reply = use_reply();
    reply->capacity_array(count);
    while (!keeper->check_end() && count > 0) {
        auto sub = LevelReply::get();
        if (sub == nullptr) {
            use_reply()->set_error(ErrorMemoryNotEnough);
            return ErrorMemoryNotEnough;
        }
        auto keyData = iter->key();
        if (keeper->mKeyOffset >= 0 && size_t(keeper->mKeyOffset) <= keyData.size()) {
            sub->set_key(PureCore::StringRef(keyData.data() + keeper->mKeyOffset, keyData.size() - keeper->mKeyOffset));
        }
        auto valData = iter->value();
        sub->set_value(PureCore::StringRef(valData.data(), valData.size()));
        reply->append_array(sub);
        --count;
        if (forward) {
            iter->Next();
        } else {
            iter->Prev();
        }
    }
    return Success;
}

int LevelConnector::move_iterator(void* handler, size_t count, bool forward) {
    auto keeper = (LevelIteratorKeeper*)handler;
    if (keeper == nullptr || count == 0) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    auto iter = keeper->mIter;
    if (iter == nullptr) {
        use_reply()->set_error(ErrorLevelIteratorInvalid);
        return ErrorLevelIteratorInvalid;
    }
    while (!keeper->check_end() && count > 0) {
        --count;
        if (forward) {
            iter->Next();
        } else {
            iter->Prev();
        }
    }
    return Success;
}

void LevelConnector::clear_iterator() {
    while (!mIterList.empty()) {
        delete mIterList.pop_front_t<LevelIteratorKeeper>();
    }
    mIterMap.clear();
}

void LevelConnector::gc_iterator() {
    int64_t now = PureCore::steady_milli_s();
    while (!mIterList.empty()) {
        auto keeper = mIterList.get_front_t<LevelIteratorKeeper>();
        if (now - keeper->mKeepTime < mIterKeepTime) {
            break;
        }

        mIterMap.erase(keeper->mID);
        delete keeper;
    }
}

int LevelConnector::real_lget(PureCore::StringRef name, int64_t idx) {
    if (mContext == nullptr) {
        use_reply()->set_error(ErrorLevelStateInvalid);
        return ErrorLevelStateInvalid;
    }
    if (name.empty()) {
        use_reply()->set_error(ErrorInvalidArg);
        return ErrorInvalidArg;
    }
    bool forward = idx >= 0;
    if (idx < 0) {
        idx = -idx - 1;
    }

    calc_two_start_key(ELevelList, name, mKeyHelper1);
    calc_two_end_key(ELevelList, name, mKeyHelper2);

    auto reply = use_reply();
    if (reply == nullptr) {
        return ErrorMemoryNotEnough;
    }
    PureCore::StringRef startKey = forward ? mKeyHelper1 : mKeyHelper2;
    PureCore::StringRef endKey = forward ? mKeyHelper2 : mKeyHelper1;
    level_range(startKey, endKey, forward, [&idx, reply](PureCore::StringRef key, PureCore::StringRef val) {
        if (idx == 0) {  // found
            reply->set_key(key);
            reply->set_value(val);
            return false;
        }
        --idx;
        return true;
    });

    int err = Success;
    if (!reply->has_key() || !reply->has_value()) {
        err = ErrorLevelNotFound;
    }
    if (err != Success) {
        reply->set_error(err);
    }
    return err;
}

int LevelConnector::real_lpush(PureCore::StringRef name, PureCore::StringRef data, bool left) {
    real_lget(name, left ? 0 : -1);
    auto reply = pop_reply();
    int err = Success;
    if (reply && reply->has_key()) {  // found
        int64_t offset = calc_two_key_offset(name);
        auto key = reply->get_key();
        if (int64_t(key.size()) > offset) {
            key.reset(key.data() + offset, key.size() - offset);
            err = calc_list_key(key, !left, mKeyHelper1);
        } else {
            err = ErrorLevelKeyInvalid;
        }
    } else {  // first
        calc_list_first_key(!left, mKeyHelper1);
    }
    if (err == Success) {
        calc_two_key(ELevelList, name, mKeyHelper1, mKeyHelper2);
        return level_put(mKeyHelper2, data);
    }

    use_reply()->set_error(err);
    return err;
}

int LevelConnector::real_lpop(PureCore::StringRef name, bool left) {
    real_lget(name, left ? 0 : -1);
    auto reply = pop_reply();
    int err = Success;
    if (reply && reply->has_key()) {  // found
        return level_del(reply->get_key());
    } else {
        err = ErrorLevelNotFound;
    }
    if (err != Success) {
        use_reply()->set_error(err);
    }
    return err;
}

int LevelConnector::level_put(PureCore::StringRef key, PureCore::StringRef data) {
    auto reply = use_reply();
    reply->set_status(mContext->Put(mWriteOption, leveldb::Slice(key.data(), key.size()), leveldb::Slice(data.data(), data.size())));
    if (reply->is_error()) {
        return ErrorLevelCommandFailed;
    }
    return Success;
}

int LevelConnector::level_del(PureCore::StringRef key) {
    auto reply = use_reply();
    reply->set_status(mContext->Delete(mWriteOption, leveldb::Slice(key.data(), key.size())));
    if (reply->is_error()) {
        return ErrorLevelCommandFailed;
    }
    return Success;
}

int LevelConnector::level_get(PureCore::StringRef key) {
    auto reply = use_reply();
    auto& value = reply->use_value();
    reply->set_status(mContext->Get(mReadOption, leveldb::Slice(key.data(), key.size()), &value));
    if (reply->is_error()) {
        return ErrorLevelCommandFailed;
    }
    return Success;
}

int LevelConnector::level_write(PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> pu, PureCore::ArrayRef<PureCore::StringRef> re) {
    leveldb::WriteBatch batch;
    for (auto& data : pu) {
        batch.Put(leveldb::Slice(data.first.data(), data.first.size()), leveldb::Slice(data.second.data(), data.second.size()));
    }
    for (auto& data : re) {
        batch.Delete(leveldb::Slice(data.data(), data.size()));
    }

    auto reply = use_reply();
    reply->set_status(mContext->Write(mWriteOption, &batch));
    if (reply->is_error()) {
        return ErrorLevelCommandFailed;
    }
    return Success;
}

void LevelConnector::level_range(PureCore::StringRef start, PureCore::StringRef end, bool forward,
                                 std::function<bool(PureCore::StringRef, PureCore::StringRef)> call) {
    if (mContext == nullptr || !call) {
        return;
    }
    leveldb::ReadOptions op;
    op.fill_cache = false;
    op.verify_checksums = mReadOption.verify_checksums;
    op.snapshot = mContext->GetSnapshot();
    auto iter = mContext->NewIterator(op);
    if (start.empty()) {
        if (forward) {
            iter->SeekToFirst();
        } else {
            iter->SeekToLast();
        }

    } else {
        iter->Seek(leveldb::Slice(start.data(), start.size()));
        if (!forward) {
            if (iter->Valid()) {
                iter->Prev();
            } else {
                iter->SeekToLast();
            }
        }
    }
    const leveldb::Slice endKey(end.data(), end.size());
    while (iter->Valid()) {
        auto key = iter->key();
        auto value = iter->value();
        if (endKey.size() > 0) {
            if (forward) {
                if (key.compare(endKey) >= 0) {
                    break;
                }
            } else {
                if (key.compare(endKey) <= 0) {
                    break;
                }
            }
        }
        if (!call(PureCore::StringRef(key.data(), key.size()), PureCore::StringRef(value.data(), value.size()))) {
            break;
        }
        if (forward) {
            iter->Next();
        } else {
            iter->Prev();
        }
    }
    mContext->ReleaseSnapshot(op.snapshot);
    delete iter;
}

LevelReply* LevelConnector::use_reply() {
    if (mReply) {
        mReply->clear();
    } else {
        mReply = LevelReply::get();
    }
    return mReply.get();
}

void LevelConnector::reset_replay(LevelReply* reply) { mReply = reply; }

void LevelConnector::tick_backup() {
    if (mBackupHandler == nullptr) {
        return;
    }
    LevelBackupHanler* bk = (LevelBackupHanler*)mBackupHandler;
    if (bk->is_finish()) {
        cancel_backup();
        return;
    }
    int err = bk->backup();
    if (err != Success) {
        PureError("LevelConnector::tick_backup failed {}", get_error_desc(err));
        cancel_backup();
    }
}

void LevelConnector::cancel_backup() {
    if (mBackupHandler == nullptr) {
        return;
    }
    LevelBackupHanler* bk = (LevelBackupHanler*)mBackupHandler;
    if (bk->is_finish()) {
        PureInfo("Level finish backup cost {} S", PureCore::steady_milli_s() - bk->mStartTime / 1000);
    } else {
        PureInfo("Level cancel backup cost {} S", PureCore::steady_milli_s() - bk->mStartTime / 1000);
    }
    delete bk;
    mBackupHandler = nullptr;
}

}  // namespace PureDb
