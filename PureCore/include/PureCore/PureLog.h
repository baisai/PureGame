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

#include "PureCore/PureLogger.h"

#define PureLog(lv, ft, ...) PureCore::PureLogger::inst()->log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__}, lv, ft, ##__VA_ARGS__)

#define PureTrace(ft, ...) PureLog(spdlog::level::trace, ft, ##__VA_ARGS__)
#define PureDebug(ft, ...) PureLog(spdlog::level::debug, ft, ##__VA_ARGS__)
#define PureInfo(ft, ...) PureLog(spdlog::level::info, ft, ##__VA_ARGS__)
#define PureWarn(ft, ...) PureLog(spdlog::level::warn, ft, ##__VA_ARGS__)
#define PureError(ft, ...) PureLog(spdlog::level::err, ft, ##__VA_ARGS__)
