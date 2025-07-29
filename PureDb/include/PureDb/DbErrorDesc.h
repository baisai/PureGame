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

#include "PureDb/PureDbLib.h"

#include <string>

#define PUREDB_ERROR_MAP(XX)                                       \
    XX(ErrorCoreBufferFailed, "Core Buffer Operator Failed")       \
    XX(ErrorNullPointer, "The Pointer Is Null")                    \
    XX(ErrorMemoryNotEnough, "The Memory Not Enough")              \
    XX(ErrorInvalidArg, "The Arg Is Invalid")                      \
    XX(ErrorRedisStateInvalid, "The Redis State Invalid")          \
    XX(ErrorRedisConnectFailed, "The Redis Connect Failed")        \
    XX(ErrorRedisAuthFailed, "The Redis Auth Failed")              \
    XX(ErrorRedisSelectDbFailed, "The Redis Select Db Failed")     \
    XX(ErrorRedisGetReplyFailed, "Get Redis Get Reply Failed")     \
    XX(ErrorRedisGetReplyError, "Get Redis Get Reply Error")       \
    XX(ErrorRedisCommandFailed, "The Redis Command Failed")        \
    XX(ErrorRedisReplyError, "The Redis Reply Error")              \
    XX(ErrorMysqlFetchFailed, "The Mysql Fetch Failed")            \
    XX(ErrorMysqlStateInvalid, "The Mysql State Invalid")          \
    XX(ErrorMysqlConnectFailed, "The Mysql Connect Failed")        \
    XX(ErrorMysqlQueryFailed, "The Mysql Query Failed")            \
    XX(ErrorMysqlTransFailed, "The Mysql Trans Failed")            \
    XX(ErrorBsonInitFailed, "The Bson Init Failed")                \
    XX(ErrorMongoStateInvalid, "The Mongo State Invalid")          \
    XX(ErrorMongoConnectFailed, "The Mongo Connect Failed")        \
    XX(ErrorMongoBulkStateInvalid, "The Mongo Bulk State Invalid") \
    XX(ErrorLevelStateInvalid, "The LevelDb State Invalid")        \
    XX(ErrorLevelOpenFailed, "The LevelDb Open Failed")            \
    XX(ErrorLevelCommandFailed, "The LevelDb Command Failed")      \
    XX(ErrorLevelIteratorInvalid, "The LevelDb Iterator Invalid")  \
    XX(ErrorLevelKeyInvalid, "The LevelDb Key Invalid")            \
    XX(ErrorLevelNotFound, "The LevelDb Key Not Found")            \
    XX(ErrorLevelBackupFailed, "The LevelDb Backup Failed")        \
    XX(ErrorLevelRevertFailed, "The LevelDb Revert Failed")        \
    XX(ErrorSqliteNotData, "The Sqlite Not Data")                  \
    XX(ErrorSqliteInvalidType, "The Sqlite Type Invalid")          \
    XX(ErrorSqliteStateInvalid, "The Sqlite State Invalid")        \
    XX(ErrorSqliteOpenFailed, "The Sqlite Open Failed")            \
    XX(ErrorSqliteQueryFailed, "The Sqlite Query Failed")          \
    XX(ErrorSqliteFetchFailed, "The Sqlite Fetch Failed")          \
    XX(ErrorSqliteTransFailed, "The Sqlite Trans Failed")          \
    XX(ErrorSqliteBackupFailed, "The Sqlite Backup Failed")        \
    XX(ErrorRepeatDbConnector, "The Db Connector Repeat")          \
    XX(ErrorDbConnectorNotFound, "The Db Connector Not Found")     \
    XX(ErrorDbReplyNotFound, "The Db Reply Not Found")             \
    XX(ErrorDbAsyncReqFailed, "The Db Async Req Failed")

namespace PureDb {
enum EPureDbErrorCode {
    Success = 0,
#ifdef PUREDB_ERRORCODE_BEGIN
    __PureDbErrorCodeSeparate = PUREDB_ERRORCODE_BEGIN - 1,
#endif
#define XX(code, desc) code,
    PUREDB_ERROR_MAP(XX)
#undef XX
};

PUREDB_API std::string get_error_desc(int err);
}  // namespace PureDb
