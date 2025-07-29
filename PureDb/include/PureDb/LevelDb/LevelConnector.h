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

#include "PureCore/PureCoreLib.h"
#include "PureCore/ArrayRef.h"
#include "PureCore/Buffer/DynamicBuffer.h"
#include "PureCore/IncrIDGen.h"
#include "PureCore/NodeList.h"
#include "PureMsg/MsgClass.h"
#include "PureDb/PureDbLib.h"
#include "PureDb/LevelDb/LevelReply.h"

#include "leveldb/env.h"
#include "leveldb/db.h"

#include <unordered_map>
#include <functional>

namespace PureDb {
class LevelLogger : public leveldb::Logger {
public:
    LevelLogger();
    virtual ~LevelLogger() = default;

    virtual void Logv(const char* format, va_list ap);

private:
    PureCore::DynamicBuffer mBuffer;
    PURE_DISABLE_COPY(LevelLogger)
};

struct PUREDB_API LevelConfig {
    std::string mPath;
    bool mEnableLog = false;
    bool mCreateIfMiss = true;
    bool mErrorIfExists = false;
    bool mParanoidCheck = false;
    size_t mCacheSize = 16 * 1024 * 1024;
    int mBloomFilter = 10;
    size_t mWriteBufferSize = 16 * 1024 * 1024;
    int mMaxOpenFiles = 1000;
    size_t mBlockSize = 16 * 1024;
    size_t mMaxFileSize = 16 * 1024 * 1024;

    bool mReadChecksums = false;
    bool mReadFillCache = true;
    bool mWriteSync = false;

    int64_t mIterKeepTime = 10 * 60 * 1000;

    PUREMSG_CLASS(mPath, mEnableLog, mCreateIfMiss, mErrorIfExists, mParanoidCheck, mCacheSize, mBloomFilter, mWriteBufferSize, mMaxOpenFiles, mBlockSize,
                  mMaxFileSize, mReadChecksums, mReadFillCache, mWriteSync)
};

enum ELevelBackupCompress : int8_t {
    ELevelCompressLz4 = 0,
    ELevelCompressZip = 1,
};

class PUREDB_API LevelConnector {
public:
    LevelConnector() = default;
    ~LevelConnector();

    int connect(const LevelConfig& cfg);
    void close();

    void update();

    int vset(PureCore::StringRef name, PureCore::StringRef data);
    int vdel(PureCore::StringRef name);
    int vget(PureCore::StringRef name);
    int vmset(PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas);
    int vmdel(PureCore::ArrayRef<PureCore::StringRef> names);
    int vmget(PureCore::ArrayRef<PureCore::StringRef> names);
    size_t v_approximate_size(PureCore::StringRef name);
    int64_t create_v_iterator(PureCore::StringRef start, PureCore::StringRef end, bool forward = true);

    // name not \0 safe
    int hset(PureCore::StringRef name, PureCore::StringRef key, PureCore::StringRef data);
    int hdel(PureCore::StringRef name, PureCore::StringRef key);
    int hget(PureCore::StringRef name, PureCore::StringRef key);
    int hmset(PureCore::StringRef name, PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> datas);
    int hmdel(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int hmget(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int hgetall(PureCore::StringRef name);
    int hdrop(PureCore::StringRef name);
    size_t h_approximate_size(PureCore::StringRef name);
    int64_t create_h_iterator(PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end, bool forward = true);

    // name not \0 safe
    int sset(PureCore::StringRef name, PureCore::StringRef key);
    int sdel(PureCore::StringRef name, PureCore::StringRef key);
    bool sget(PureCore::StringRef name, PureCore::StringRef key);
    int smset(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int smdel(PureCore::StringRef name, PureCore::ArrayRef<PureCore::StringRef> keys);
    int sgetall(PureCore::StringRef name);
    int sdrop(PureCore::StringRef name);
    size_t s_approximate_size(PureCore::StringRef name);
    int64_t create_s_iterator(PureCore::StringRef name, PureCore::StringRef start, PureCore::StringRef end, bool forward = true);

    // name not \0 safe
    int lset(PureCore::StringRef name, int64_t idx, PureCore::StringRef data);
    int llpush(PureCore::StringRef name, PureCore::StringRef data);
    int llpop(PureCore::StringRef name);
    int lrpush(PureCore::StringRef name, PureCore::StringRef data);
    int lrpop(PureCore::StringRef name);
    int lget(PureCore::StringRef name, int64_t idx);
    int lrange(PureCore::StringRef name, int64_t start, int64_t end);
    int ldel(PureCore::StringRef name, int64_t count, PureCore::StringRef data);
    int ltrim(PureCore::StringRef name, int64_t start, int64_t end);
    int lgetall(PureCore::StringRef name);
    int ldrop(PureCore::StringRef name);
    size_t l_approximate_size(PureCore::StringRef name);
    int64_t create_l_iterator(PureCore::StringRef name, bool forward = true);

    size_t approximate_size();
    int property(PureCore::StringRef name);
    void compact();

    // batch < 0, all
    int backup_to(const char* path, int64_t batch, ELevelBackupCompress compress = ELevelCompressLz4);
    bool is_backuping() const;
    int revert_from(const char* path);

    int iterator(int64_t id, size_t count, bool forward = true);
    bool remove_iterator(int64_t id);

    LevelReplyPtr pop_reply();
    LevelReply* get_reply();
    PureCore::StringRef get_last_error() const;

private:
    int64_t create_range_iterator(int64_t keyOffset, PureCore::StringRef start, PureCore::StringRef end, bool forward = true);
    void* create_iterator();
    int read_iterator(void* handler, size_t count, bool forward = true);
    int move_iterator(void* handler, size_t count, bool forward = true);
    void clear_iterator();
    void gc_iterator();

    int real_lget(PureCore::StringRef name, int64_t idx);
    int real_lpush(PureCore::StringRef name, PureCore::StringRef data, bool left);
    int real_lpop(PureCore::StringRef name, bool left);

    int level_put(PureCore::StringRef key, PureCore::StringRef data);
    int level_del(PureCore::StringRef key);
    int level_get(PureCore::StringRef key);
    int level_write(PureCore::ArrayRef<std::pair<PureCore::StringRef, PureCore::StringRef>> pu, PureCore::ArrayRef<PureCore::StringRef> re);
    void level_range(PureCore::StringRef start, PureCore::StringRef end, bool forward, std::function<bool(PureCore::StringRef, PureCore::StringRef)> call);

    LevelReply* use_reply();
    void reset_replay(LevelReply* reply);

    void tick_backup();
    void cancel_backup();

private:
    leveldb::Options mOptions;
    leveldb::ReadOptions mReadOption;
    leveldb::WriteOptions mWriteOption;
    leveldb::DB* mContext = nullptr;
    LevelReplyPtr mReply;

    PureCore::IncrIDGen mIterID;
    PureCore::NodeList mIterList;
    std::unordered_map<int64_t, void*> mIterMap;
    int64_t mIterKeepTime = 0;

    void* mBackupHandler = nullptr;

    std::string mKeyHelper1;
    std::string mKeyHelper2;
};

}  // namespace PureDb
