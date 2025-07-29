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
#include "PureCore/StringRef.h"

#include "spdlog/spdlog.h"

namespace PureCore {
static const std::string sEmptyStr{};
class PURECORE_API PureLogger {
public:
    static PureLogger* inst();
    static void cleanup();

    PureLogger();
    ~PureLogger();

    int add_simple_file(const char* path, const std::string& style = sEmptyStr);
    int add_rotating_file(const char* path, size_t maxSize, size_t maxCount, const std::string& style = sEmptyStr);
    int add_daily_file(const char* path, int hour, int minmutes, const std::string& style = sEmptyStr);
    int add_hourly_file(const char* path, const std::string& style = sEmptyStr);
    int add_msvc(const std::string& style = sEmptyStr);
    int add_android(const std::string& tag, const std::string& style = sEmptyStr);
    int add_stdout(spdlog::color_mode m, const std::string& style = sEmptyStr);
    int add_user_sink(spdlog::sink_ptr sink, const std::string& style = sEmptyStr);
    void clear();

    void start(const std::string& name, size_t asyncSize = 0);
    int set_style(const std::string& style);

    int set_level(spdlog::level::level_enum level);
    spdlog::level::level_enum get_level() const;
    int set_flush_level(spdlog::level::level_enum level);
    spdlog::level::level_enum get_flush_level() const;
    int flush();

    void log(spdlog::level::level_enum lvl, StringRef msg);

    template <typename... Args>
    inline void log(spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        if (mLogger != nullptr) {
            mLogger->log(lvl, fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    inline void log(spdlog::source_loc source, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        if (mLogger != nullptr) {
            mLogger->log(source, lvl, fmt, std::forward<Args>(args)...);
        }
    }

    inline void trace(StringRef msg) { log(spdlog::level::trace, msg); }

    template <typename... Args>
    inline void trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(spdlog::level::trace, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void trace(spdlog::source_loc source, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(source, spdlog::level::trace, fmt, std::forward<Args>(args)...);
    }

    inline void debug(StringRef msg) { log(spdlog::level::debug, msg); }

    template <typename... Args>
    inline void debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(spdlog::level::debug, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void debug(spdlog::source_loc source, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(source, spdlog::level::debug, fmt, std::forward<Args>(args)...);
    }

    inline void info(StringRef msg) { log(spdlog::level::trace, msg); }

    template <typename... Args>
    inline void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(spdlog::level::info, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void info(spdlog::source_loc source, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(source, spdlog::level::info, fmt, std::forward<Args>(args)...);
    }

    inline void warn(StringRef msg) { log(spdlog::level::warn, msg); }

    template <typename... Args>
    inline void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(spdlog::level::warn, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void warn(spdlog::source_loc source, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(source, spdlog::level::warn, fmt, std::forward<Args>(args)...);
    }

    inline void error(StringRef msg) { log(spdlog::level::trace, msg); }

    template <typename... Args>
    inline void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(spdlog::level::err, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void error(spdlog::source_loc source, spdlog::format_string_t<Args...> fmt, Args&&... args) {
        log(source, spdlog::level::err, fmt, std::forward<Args>(args)...);
    }

private:
    int create_log_dir(const char* file);
    spdlog::filename_t path_from_utf8(const char* path);

private:
    std::shared_ptr<spdlog::logger> mLogger{};
    std::vector<spdlog::sink_ptr> mSinks{};
    std::shared_ptr<spdlog::details::thread_pool> mTp{};

    static PureLogger* sInst;
    PURE_DISABLE_COPY(PureLogger)
};
}  // namespace PureCore
