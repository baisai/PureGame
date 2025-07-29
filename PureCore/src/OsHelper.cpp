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

#include "PureCore/OsHelper.h"
#include "PureCore/UtfHelper.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureCore/ByteOrder.h"

#include <stdio.h>
#include <string>
#include <sys/stat.h>

#ifndef _WIN32
#include <sys/sysinfo.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#else
#include "windows.h"
#endif

#include <thread>
#include <chrono>
#include <fstream>

namespace PureCore {
tm localtime(time_t time_tt) {
#ifdef _WIN32
    tm t;
    ::localtime_s(&t, &time_tt);
#else
    tm t;
    ::localtime_r(&time_tt, &t);
#endif
    return t;
}

tm localtime() { return localtime(time(nullptr)); }

tm gmtime(time_t time_tt) {
#ifdef _WIN32
    tm t;
    ::gmtime_s(&t, &time_tt);
#else
    tm t;
    ::gmtime_r(&time_tt, &t);
#endif
    return t;
}

tm gmtime() { return gmtime(time(nullptr)); }

void gettimeofday(struct timeval* tv) {
#ifdef _WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define delta 116444736000000000Ui64
#else
#define delta 116444736000000000ULL
#endif
#define secOffSet 10000000u
    if (tv != nullptr) {
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);
        uint64_t present = fileTime.dwHighDateTime;
        present <<= 32;
        present |= fileTime.dwLowDateTime;
        // Subtract the difference
        present -= delta;
        tv->tv_sec = static_cast<long>(present / secOffSet);
        tv->tv_usec = static_cast<long>((present % secOffSet) / 10u);
    }
#undef delta
#undef secOffSet
#else
    ::gettimeofday(tv, nullptr);
#endif
}

int utc_minutes_offset(const tm t) {
#ifdef _WIN32
#if _WIN32_WINNT < _WIN32_WINNT_WS08
    TIME_ZONE_INFORMATION tzinfo;
    uint32_t rv = GetTimeZoneInformation(&tzinfo);
#else
    DYNAMIC_TIME_ZONE_INFORMATION tzinfo;
    uint32_t rv = GetDynamicTimeZoneInformation(&tzinfo);
#endif
    if (rv == TIME_ZONE_ID_INVALID) return 0;

    int offset = -tzinfo.Bias;
    if (t.tm_isdst)
        offset -= tzinfo.DaylightBias;
    else
        offset -= tzinfo.StandardBias;
    return offset;
#else

#if defined(sun) || defined(__sun)
    // 'tm_gmtoff' field is BSD extension and it's missing on SunOS/Solaris
    struct sun_helper {
            static long int calculate_gmt_offset(const tm& localtm, const tm& gmtm))
			{
                int local_year = localtm.tm_year + (1900 - 1);
                int gmt_year = gmtm.tm_year + (1900 - 1);

                long int days = (
                    // difference in day of year
                    localtm.tm_yday -
                    gmtm.tm_yday

                    // + intervening leap days
                    + ((local_year >> 2) - (gmt_year >> 2)) - (local_year / 100 - gmt_year / 100) +
                    ((local_year / 100 >> 2) - (gmt_year / 100 >> 2))

                    // + difference in years * 365 */
                    + (long int)(local_year - gmt_year) * 365);

                long int hours = (24 * days) + (localtm.tm_hour - gmtm.tm_hour);
                long int mins = (60 * hours) + (localtm.tm_min - gmtm.tm_min);
                long int secs = (60 * mins) + (localtm.tm_sec - gmtm.tm_sec);

                return secs;
            }
    };

    long int offset_seconds = sun_helper::calculate_gmt_offset(t, gmtime(mktime(t)));
#else
    long int offset_seconds = t.tm_gmtoff;
#endif

    return static_cast<int>(offset_seconds / 60);
#endif
}

int64_t steady_s() { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

int64_t steady_milli_s() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

int64_t steady_micro_s() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

int64_t system_s() { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

int64_t system_milli_s() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

int64_t system_micro_s() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

FILE* fopen(const char* filename, const char* mode) {
#ifdef _WIN32
    std::wstring wFilename, wMode;
    if (string_to_wstring(filename, wFilename) != Success) {
        return nullptr;
    }
    if (string_to_wstring(mode, wMode) != Success) {
        return nullptr;
    }
    FILE* fp = ::_wfsopen(wFilename.c_str(), wMode.c_str(), _SH_DENYWR);
#else
    FILE* fp = ::fopen(filename, mode);
#endif
    return fp;
}

int remove_file(const char* filename) {
#if defined(_WIN32)
    std::wstring wFilename;
    if (string_to_wstring(filename, wFilename) != Success) {
        return -1;
    }
    return ::_wremove(wFilename.c_str());
#else
    return ::remove(filename);
#endif
}
int rename_file(const char* oldName, const char* newName) {
#if defined(_WIN32)
    std::wstring wOldName, wNewName;
    if (string_to_wstring(oldName, wOldName) != Success) {
        return -1;
    }
    if (string_to_wstring(newName, wNewName) != Success) {
        return -1;
    }
    return ::_wrename(wOldName.c_str(), wNewName.c_str());
#else
    return ::rename(oldName, newName);
#endif
}

bool file_exist(const char* filename) {
#ifdef _WIN32
    std::wstring wFilename;
    if (string_to_wstring(filename, wFilename) != Success) {
        return false;
    }
    uint32_t attribs = ::GetFileAttributesW(wFilename.c_str());
    if (attribs == INVALID_FILE_ATTRIBUTES || attribs & FILE_ATTRIBUTE_DIRECTORY) return false;
#else  // common linux/unix all have the stat system call
    struct stat st;
    if (::stat(filename, &st) || st.st_mode & S_IFDIR) return false;
#endif
    return true;
}

bool dir_exist(const char* dirname) {
#ifdef _WIN32
    std::wstring wDirname;
    if (string_to_wstring(dirname, wDirname) != Success) {
        return false;
    }
    uint32_t attribs = ::GetFileAttributesW(wDirname.c_str());

    if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) return false;
#else  // common linux/unix all have the stat system call
    struct stat st;
    if (::stat(dirname, &st) || !(st.st_mode & S_IFDIR)) return false;
#endif
    return true;
}

std::string get_parent_path(const char* path) {
#ifdef _WIN32
    std::wstring realPath;
    if (string_to_wstring(path, realPath) != Success) {
        return std::string();
    }
    const wchar_t* p = L"/\\";
#else
    std::string realPath(path);
    const char* p = "/\\";
#endif
    if (realPath.empty()) {
        return std::string();
    }
    size_t idx = realPath.find_last_of(p);
    if (idx == realPath.size() - 1) {
        realPath.resize(realPath.size() - 1);
        idx = realPath.find_last_of(p);
    }
    if (idx == std::string::npos) {
        return std::string();
    }
#ifdef _WIN32
    std::string result;
    wstring_to_string(realPath.substr(0, idx + 1), result);
    return result;
#else
    return realPath.substr(0, idx + 1);
#endif
}

std::string get_path_leaf(const char* path) {
#ifdef _WIN32
    std::wstring realPath;
    if (string_to_wstring(path, realPath) != Success) {
        return std::string();
    }
    const wchar_t* p = L"/\\";
#else
    std::string realPath(path);
    const char* p = "/\\";
#endif
    if (realPath.empty()) {
        return std::string();
    }
    size_t idx = realPath.find_last_of(p);
    if (idx == realPath.size() - 1) {
        realPath.resize(realPath.size() - 1);
        idx = realPath.find_last_of(p);
    }
#ifdef _WIN32
    if (idx == std::string::npos) {
        return std::string(path);
    }
    std::string result;
    wstring_to_string(realPath.substr(idx + 1, realPath.size()), result);
    return result;
#else
    if (idx == std::string::npos) {
        return realPath;
    }
    return realPath.substr(idx + 1, realPath.size());
#endif
}

std::string get_file_ext(const char* path) {
#ifdef _WIN32
    std::wstring realPath;
    if (string_to_wstring(path, realPath) != Success) {
        return std::string();
    }
    const wchar_t* p = L".";
#else
    std::string realPath(path);
    const char* p = ".";
#endif
    if (realPath.empty()) {
        return std::string();
    }
    size_t idx = realPath.find_last_of(p);
    if (idx == realPath.size() - 1) {
        realPath.resize(realPath.size() - 1);
        idx = realPath.find_last_of(p);
    }
#ifdef _WIN32
    if (idx == std::string::npos) {
        return std::string();
    }
    std::string result;
    wstring_to_string(realPath.substr(idx + 1, realPath.size()), result);
    return result;
#else
    if (idx == std::string::npos) {
        return realPath;
    }
    return realPath.substr(idx + 1, realPath.size());
#endif
}

std::string get_trim_path(const char* path) {
#ifdef _WIN32
    std::wstring realPath;
    std::vector<std::wstring> paths;
    if (string_to_wstring(path, realPath) != Success) {
        return std::string();
    }
    const wchar_t* p = L"/\\";
    const wchar_t* up = L"..";
    const wchar_t* cur = L".";
    const wchar_t r = L'/';
#else
    std::string realPath(path);
    std::vector<std::string> paths;
    const char* p = "/\\";
    const char* up = "..";
    const char* cur = ".";
    const char r = '/';
#endif
    if (realPath.empty()) {
        return std::string();
    }
    size_t idx = 0;
    do {
        size_t next = realPath.find_first_of(p, idx);
        if (next != std::string::npos) {
            paths.push_back(realPath.substr(idx, next - idx));
            idx = next + 1;
        } else {
            paths.push_back(realPath.substr(idx));
            idx = realPath.size();
        }
    } while (idx < realPath.size());
    if (!paths.empty() && paths[0].empty()) {
        paths[0].push_back(r);
    }
    for (size_t i = 1; i < paths.size(); ++i) {
        if (paths[i] == cur) {
            paths[i].clear();
        } else if (paths[i] == up) {
            for (size_t j = i - 1; j > 0; --j) {
                if (paths[j].empty()) {
                    continue;
                }
                paths[j].clear();
                break;
            }
            paths[i].clear();
        }
    }
#ifdef _WIN32
    std::wstring newPaths;
    for (size_t i = 0; i < paths.size(); ++i) {
        if (paths[i].empty()) {
            continue;
        }
        if (i > 0) {
            newPaths.push_back(L'/');
        }
        newPaths.append(paths[i]);
    }
    std::string result;
    wstring_to_string(newPaths, result);
    return result;
#else
    std::string result;
    for (size_t i = 1; i < paths.size(); ++i) {
        if (paths[i].empty()) {
            continue;
        }
        result.push_back('/');
        result.append(paths[i]);
    }
    return result;
#endif
}

bool create_dir(const char* dirname) {
    if (dir_exist(dirname)) return true;

    std::string parentPath = get_parent_path(dirname);
    if (!parentPath.empty() && !dir_exist(parentPath.c_str())) {
        if (!create_dir(parentPath.c_str())) return false;
    }
#ifdef _WIN32
    std::wstring wDirname;
    if (string_to_wstring(dirname, wDirname) != Success) {
        return false;
    }
    bool success = (CreateDirectoryW(wDirname.c_str(), 0) == TRUE) || (GetLastError() == ERROR_ALREADY_EXISTS);
#else
    bool success = mkdir(dirname, S_IRWXU) == 0 || errno == EEXIST;
#endif
    return success;
}

bool is_absolute_path(const char* path) {
    if (!path || path[0] == '\0') {
        return false;
    }
    return path[0] == '/' || strchr(path, ':');
}

size_t file_size(FILE* fp) {
    if (fp == nullptr) return 0;
#ifdef _WIN32
    int fd = _fileno(fp);
#if _WIN64  // 64 bits
    struct _stat64 st;
    if (_fstat64(fd, &st) == 0) return st.st_size;

#else  // windows 32 bits
    struct _stat st;
    if (_fstat(fd, &st) == 0) return st.st_size;
#endif

#else  // unix
    int fd = fileno(fp);
    // 64 bits(but not in osx, where fstat64 is deprecated)
#if !defined(__FreeBSD__) && !defined(__APPLE__) && (defined(__x86_64__) || defined(__ppc64__))
    struct stat64 st;
    if (fstat64(fd, &st) == 0) return st.st_size;
#else  // unix 32 bits or osx
    struct stat st;
    if (fstat(fd, &st) == 0) return st.st_size;
#endif
#endif
    return 0;
}

int read_file(DynamicBuffer& buffer, const char* path) {
    std::ifstream ifs;
    ifs.open(path, std::ios::binary | std::ios::in);
    if (!ifs.is_open()) {
        return ErrorOpenFileFailed;
    }
    while (!ifs.eof()) {
        if (buffer.free_size() <= 0) {
            int err = buffer.ensure_buffer(buffer.size() + 1024 * 1024);
            if (err != Success) {
                ifs.close();
                return err;
            }
        }
        auto data = buffer.free_buffer();
        ifs.read(data.data(), data.size());
        buffer.write_pos(buffer.write_pos() + ifs.gcount());
        if (ifs.bad()) {
            ifs.close();
            return ErrorReadFileFailed;
        }
    }
    ifs.close();
    return Success;
}

int write_file(DataRef data, const char* path) {
    std::ofstream ofs;
    ofs.open(path, std::ios::binary | std::ios::out);
    if (!ofs.is_open()) {
        return ErrorOpenFileFailed;
    }
    ofs.write(data.data(), data.size());
    if (!ofs.good()) {
        ofs.close();
        return ErrorWriteFileFailed;
    }
    ofs.close();
    return Success;
}

bool enum_dir_files(const char* path, std::vector<std::string>& outFiles, bool dir, bool recursion) {
    if (!path || path[0] == '\0') {
        return false;
    }
    std::string filePath = path;
    char lastChar = filePath.back();
    if (lastChar != '/' && lastChar != '\\') {
        filePath.push_back('/');
    }
#ifdef _WIN32
    filePath.append("*.*");
    std::wstring wFilePath;
    if (string_to_wstring(filePath, wFilePath) != Success) {
        return false;
    }
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(wFilePath.c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) {
                continue;
            }
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (dir) {
                    std::string filename;
                    if (wstring_to_string(findFileData.cFileName, filename) == Success) {
                        outFiles.push_back(filename);
                    }
                }
                if (recursion) {
                    std::string fullPath = filePath;
                    fullPath.resize(fullPath.size() - 3);
                    std::string subDir;
                    if (wstring_to_string(findFileData.cFileName, subDir) != Success) {
                        return false;
                    }
                    fullPath.append(subDir);
                    std::vector<std::string> subFiles;
                    bool result = enum_dir_files(fullPath.c_str(), subFiles, dir, recursion);
                    if (result) {
                        subDir.push_back('/');
                        size_t len = subDir.size();
                        for (size_t i = 0; i < subFiles.size(); ++i) {
                            subDir.resize(len);
                            subDir.append(subFiles[i]);
                            outFiles.push_back(subDir);
                        }
                    } else {
                        return result;
                    }
                }
            } else {
                std::string filename;
                if (wstring_to_string(findFileData.cFileName, filename) == Success) {
                    outFiles.push_back(filename);
                }
            }
        } while (FindNextFileW(hFind, &findFileData));
    }
    return true;
#else
    DIR* pDir = opendir(filePath.c_str());
    if (pDir == nullptr) {
        return false;
    }
    struct dirent* pDirent = nullptr;
    while ((pDirent = readdir(pDir)) != nullptr) {
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) {
            continue;
        }
        if (pDirent->d_type == DT_DIR) {
            if (dir) {
                outFiles.push_back(pDirent->d_name);
            }
            if (recursion) {
                std::string fullPath = filePath;
                fullPath.append(pDirent->d_name);
                std::vector<std::string> subFiles;
                bool result = enum_dir_files(fullPath.c_str(), subFiles, dir, recursion);
                if (result) {
                    std::string subFile(pDirent->d_name);
                    subFile.push_back('/');
                    size_t len = subFile.size();
                    for (size_t i = 0; i < subFiles.size(); ++i) {
                        subFile.resize(len);
                        subFile.append(subFiles[i]);
                        outFiles.push_back(subFile);
                    }
                } else {
                    return result;
                }
            }
        } else {
            outFiles.push_back(pDirent->d_name);
        }
    }
    closedir(pDir);
    return true;
#endif
}

std::string errno_str(int err_num) {
    char buf[256];
    size_t buf_size = sizeof(buf);

#ifdef _WIN32
    if (::strerror_s(buf, buf_size, err_num) == 0)
        return std::string(buf);
    else
        return std::string("Unkown error");

#elif defined(__FreeBSD__) || defined(__APPLE__) || defined(__ANDROID__) || ((_POSIX_C_SOURCE >= 200112L) && !_GNU_SOURCE)  // posix version

    if (::strerror_r(err_num, buf, buf_size) == 0)
        return std::string(buf);
    else
        return std::string("Unkown error");

#else  // gnu version (might not use the given buf, so its retval pointer must
       // be used)
    return std::string(::strerror_r(err_num, buf, buf_size));
#endif
}

int64_t get_used_virtual() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
#else
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    int64_t virtualMemUsed = memInfo.totalram - memInfo.freeram;
    // Add other values in next statement to avoid int overflow on right hand
    // side...
    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;

    return virtualMemUsed;
#endif
}

int64_t get_total_virtual() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    return memInfo.ullTotalPageFile;
#else
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    int64_t totalVirtualMem = memInfo.totalram;
    // Add other values in next statement to avoid int overflow on right hand
    // side...
    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;

    return totalVirtualMem;
#endif
}

int64_t get_used_physical() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    return memInfo.ullTotalPhys - memInfo.ullAvailPhys;
#else
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    int64_t physMemUsed = memInfo.totalram - memInfo.freeram;
    // Multiply in next statement to avoid int overflow on right hand side...
    physMemUsed *= memInfo.mem_unit;

    return physMemUsed;
#endif
}

int64_t get_total_physical() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    return memInfo.ullTotalPhys;
#else
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    long long totalPhysMem = memInfo.totalram;
    // Multiply in next statement to avoid int overflow on right hand side...
    totalPhysMem *= memInfo.mem_unit;

    return totalPhysMem;
#endif
}

std::string get_bin_path() {
    std::string binPath;
    binPath.resize(8192);
#ifdef _WIN32
    ptrdiff_t nLen = ::GetModuleFileName(nullptr, &binPath[0], static_cast<uint32_t>(binPath.size()));
    for (ptrdiff_t i = 0; i < nLen; ++i) {
        if (binPath[i] == '\\') {
            binPath[i] = '/';
        }
    }
#else
    ptrdiff_t nLen = readlink("/proc/self/exe", &binPath[0], binPath.size());
#endif  // _WIN32
    binPath.resize(nLen);
    return binPath;
}

void sleep(int64_t milliSeconds) { std::this_thread::sleep_for(std::chrono::milliseconds(milliSeconds)); }

void yeild() { std::this_thread::yield(); }

int64_t get_thread_id() {
#ifdef _WIN32
    return GetCurrentThreadId();
#elif __FreeBSD__
    return pthread_getthreadid_np();
#else
    return pthread_self();
#endif
}

int64_t get_process_id() {
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

uint32_t get_cpu_core() {
#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return uint32_t(si.dwNumberOfProcessors);
#else
    return uint32_t(sysconf(_SC_NPROCESSORS_ONLN));
#endif
}

bool operator==(const tm& tm1, const tm& tm2) {
    return (tm1.tm_sec == tm2.tm_sec && tm1.tm_min == tm2.tm_min && tm1.tm_hour == tm2.tm_hour && tm1.tm_mday == tm2.tm_mday && tm1.tm_mon == tm2.tm_mon &&
            tm1.tm_year == tm2.tm_year && tm1.tm_isdst == tm2.tm_isdst);
}
bool operator!=(const tm& tm1, const tm& tm2) { return !(tm1 == tm2); }

uint32_t four_char_to_u32(const char src[sizeof(uint32_t)]) {
    uint32_t dest = 0;
    _pure_byte_load32(uint32_t, src, &dest);
    return dest;
}
void u32_to_four_char(uint32_t src, char dest[sizeof(uint32_t)]) { _pure_byte_store32(dest, src); }

bool is_ip_v4(const char* ip) {
    if (strchr(ip, ':') == nullptr) {  // ipv4
        return true;
    }
    return false;
}

bool is_ip_v6(const char* ip) { return !is_ip_v4(ip); }

}  // namespace PureCore
