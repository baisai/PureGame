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

#include "PureCore/IncrIDGen.h"

#include <map>
#include <functional>

namespace PureCore {
template <typename... Args>
class Event {
public:
    Event() = default;

    template <typename T>
    int64_t bind(T* obj, bool (T::*func)(Args...)) {
        if (obj == nullptr) {
            return 0;
        }
        return bind([=](Args&&... args) { return (obj->*func)(std::forward<Args>(args)...); });
    }

    int64_t bind(std::function<bool(Args...)> callback) {
        int64_t id = mGen.gen_id();
        mCallbacks.emplace(id, callback);
        return id;
    }

    Event& operator+=(std::function<bool(Args...)> callback) {
        bind(callback);
        return *this;
    }

    void unbind(int64_t id) {
        auto iter = mCallbacks.find(id);
        if (iter == mCallbacks.end()) {
            return;
        }
        if (mNotifing) {
            iter->second = nullptr;
        } else {
            mCallbacks.erase(iter);
        }
    }

    void clear() {
        if (mNotifing) {
            for (auto& iter : mCallbacks) {
                iter.second = nullptr;
            }
        } else {
            mCallbacks.clear();
        }
    }

    // if return false remove self
    void notify(Args... args) {
        mNotifing = true;
        for (auto iter = mCallbacks.begin(); iter != mCallbacks.end();) {
            if (iter->second && iter->second(std::forward<Args>(args)...)) {
                ++iter;
            } else {
                iter = mCallbacks.erase(iter);
            }
        }
        mNotifing = false;
    }

private:
    std::map<int64_t, std::function<bool(Args...)>> mCallbacks;
    bool mNotifing = false;
    IncrIDGen mGen;
};
}  // namespace PureCore
