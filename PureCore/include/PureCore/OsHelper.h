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
#include "PureCore/Buffer/DynamicBuffer.h"

#include <string>
#include <vector>
#include <time.h>

#ifndef _WIN32
#include <sys/time.h>
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif  // !NOMINMAX

#include "winsock2.h"
#endif

namespace PureCore {
PURECORE_API tm localtime(time_t time_tt);
PURECORE_API tm localtime();
PURECORE_API tm gmtime(time_t time_tt);
PURECORE_API tm gmtime();
PURECORE_API void gettimeofday(struct timeval* tv);
PURECORE_API int utc_minutes_offset(const tm t = localtime());

PURECORE_API int64_t steady_s();
PURECORE_API int64_t steady_milli_s();
PURECORE_API int64_t steady_micro_s();
PURECORE_API int64_t system_s();
PURECORE_API int64_t system_milli_s();
PURECORE_API int64_t system_micro_s();

PURECORE_API FILE* fopen(const char* filename, const char* mode);
PURECORE_API int remove_file(const char* filename);
PURECORE_API int rename_file(const char* oldName, const char* newName);
PURECORE_API bool file_exist(const char* filename);
PURECORE_API bool dir_exist(const char* dirname);
PURECORE_API std::string get_parent_path(const char* path);
PURECORE_API std::string get_path_leaf(const char* path);
PURECORE_API std::string get_file_ext(const char* path);
PURECORE_API std::string get_trim_path(const char* path);
PURECORE_API bool create_dir(const char* dirname);
PURECORE_API bool is_absolute_path(const char* path);
PURECORE_API size_t file_size(FILE* fp);
PURECORE_API int read_file(DynamicBuffer& buffer, const char* path);
PURECORE_API int write_file(DataRef data, const char* path);
PURECORE_API bool enum_dir_files(const char* path, std::vector<std::string>& outFiles, bool dir, bool recursion);

PURECORE_API std::string errno_str(int err_num);

PURECORE_API int64_t get_used_virtual();
PURECORE_API int64_t get_total_virtual();
PURECORE_API int64_t get_used_physical();
PURECORE_API int64_t get_total_physical();

PURECORE_API std::string get_bin_path();

PURECORE_API void sleep(int64_t milliSeconds);
PURECORE_API void yeild();
PURECORE_API int64_t get_thread_id();
PURECORE_API int64_t get_process_id();
PURECORE_API uint32_t get_cpu_core();

PURECORE_API bool operator==(const tm& tm1, const tm& tm2);
PURECORE_API bool operator!=(const tm& tm1, const tm& tm2);

PURECORE_API uint32_t four_char_to_u32(const char src[sizeof(uint32_t)]);
PURECORE_API void u32_to_four_char(uint32_t src, char dest[sizeof(uint32_t)]);

PURECORE_API bool is_ip_v4(const char* ip);
PURECORE_API bool is_ip_v6(const char* ip);

}  // namespace PureCore
