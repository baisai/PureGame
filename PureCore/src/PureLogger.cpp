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

#include "PureCore/PureLogger.h"
#include "PureCore/CoreErrorDesc.h"
#include "PureCore/OsHelper.h"
#include "PureCore/UtfHelper.h"

#include "spdlog/async_logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/android_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/hourly_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <iostream>

namespace PureCore {

PureLogger* PureLogger::inst() {
    if (sInst == nullptr) {
        sInst = new PureLogger();
    }
    return sInst;
}

void PureLogger::cleanup() {
    if (sInst != nullptr) {
        delete sInst;
        sInst = nullptr;
    }
}

PureLogger::PureLogger() {  // normal use stdout
    add_stdout(spdlog::color_mode::automatic);
    start("Pure", 0);
}

PureLogger::~PureLogger() { clear(); }

int PureLogger::add_simple_file(const char* path, const std::string& style) {
    int err = create_log_dir(path);
    if (err != Success) {
        return err;
    }
    try {
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path_from_utf8(path));
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
    return Success;
}

int PureLogger::add_rotating_file(const char* path, size_t maxSize, size_t maxCount, const std::string& style) {
    int err = create_log_dir(path);
    if (err != Success) {
        return err;
    }
    try {
        auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path_from_utf8(path), maxSize, maxCount);
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
    return Success;
}

int PureLogger::add_daily_file(const char* path, int hour, int minmutes, const std::string& style) {
    int err = create_log_dir(path);
    if (err != Success) {
        return err;
    }
    try {
        auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(path_from_utf8(path), hour, minmutes);
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
    return Success;
}

int PureLogger::add_hourly_file(const char* path, const std::string& style) {
    int err = create_log_dir(path);
    if (err != Success) {
        return err;
    }
    try {
        auto sink = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(path_from_utf8(path));
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
    return Success;
}

int PureLogger::add_msvc(const std::string& style) {
#ifdef _MSC_VER
    try {
        auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
#endif
    return Success;
}

int PureLogger::add_android(const std::string& tag, const std::string& style) {
#ifdef __ANDROID__
    try {
        auto sink = std::make_shared<spdlog::sinks::android_sink_mt>(tag);
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
#endif
    return Success;
}

int PureLogger::add_stdout(spdlog::color_mode m, const std::string& style) {
    try {
        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(m);
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
    return Success;
}

int PureLogger::add_user_sink(spdlog::sink_ptr sink, const std::string& style) {
    try {
        if (!style.empty()) {
            sink->set_pattern(style);
        }
        mSinks.push_back(sink);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << ex.what() << std::endl;
        return ErrorLogAddSinkFailed;
    }
    return Success;
}

void PureLogger::clear() {
    mLogger.reset();
    mSinks.clear();
    mTp.reset();
}

void PureLogger::start(const std::string& name, size_t asyncSize) {
    if (asyncSize != 0) {
        mTp = std::make_shared<spdlog::details::thread_pool>(asyncSize, 1);
        mLogger = std::make_shared<spdlog::async_logger>(name, mSinks.begin(), mSinks.end(), mTp, spdlog::async_overflow_policy::block);
    } else {
        mLogger = std::make_shared<spdlog::logger>(name, mSinks.begin(), mSinks.end());
    }
    mLogger->flush_on(spdlog::level::warn);
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

int PureLogger::set_style(const std::string& style) {
    if (style.empty()) {
        return ErrorInvalidArg;
    }
    if (!mLogger) {
        return ErrorLogNotStart;
    }
    mLogger->set_pattern(style);
    return Success;
}

int PureLogger::set_level(spdlog::level::level_enum level) {
    if (!mLogger) {
        return ErrorLogNotStart;
    }
    mLogger->set_level(level);
    return Success;
}

spdlog::level::level_enum PureLogger::get_level() const {
    if (!mLogger) {
        return spdlog::level::n_levels;
    }
    return mLogger->level();
}

int PureLogger::set_flush_level(spdlog::level::level_enum level) {
    if (!mLogger) {
        return ErrorLogNotStart;
    }
    mLogger->flush_on(level);
    return Success;
}

spdlog::level::level_enum PureLogger::get_flush_level() const {
    if (!mLogger) {
        return spdlog::level::n_levels;
    }
    return mLogger->flush_level();
}

int PureLogger::flush() {
    if (!mLogger) {
        return ErrorLogNotStart;
    }
    mLogger->flush();
    return Success;
}

void PureLogger::log(spdlog::level::level_enum lvl, StringRef msg) {
    if (mLogger != nullptr) {
        mLogger->log(lvl, spdlog::string_view_t(msg.data(), msg.size()));
    }
}

int PureLogger::create_log_dir(const char* file) {
    if (file == nullptr) {
        return ErrorInvalidArg;
    }
    auto dir = get_parent_path(file);
    if (dir.empty() || !create_dir(dir.c_str())) {
        return ErrorInvalidArg;
    }
    return Success;
}

spdlog::filename_t PureLogger::path_from_utf8(const char* path) {
#ifdef _WIN32
    std::wstring wpath;
    string_to_wstring(path, wpath);
    return std::move(wpath);
#else
    return spdlog::filename_t(path);
#endif
}

PureLogger* PureLogger::sInst = nullptr;
}  // namespace PureCore