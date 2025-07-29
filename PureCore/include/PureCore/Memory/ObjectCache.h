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

#include <vector>
#include <stddef.h>

namespace PureCore {
template <typename T, size_t MaxCache>
class ObjectCache {
public:
    ObjectCache() : mObjects() {}

    ~ObjectCache() {
        for (size_t i = 0; i < mObjects.size(); ++i) {
            if (mObjects[i] == nullptr) {
                continue;
            }
            delete mObjects[i];
        }
        mObjects.clear();
    }

    template <typename... Args>
    T* get(Args&&... args) {
        if (mObjects.empty()) {
            return new T(std::forward<Args>(args)...);
        }
        T* obj = mObjects.back();
        mObjects.pop_back();
        return obj;
    }

    void free(T* p) {
        if (p == nullptr) {
            return;
        }
        if (mObjects.size() >= MaxCache) {
            p->clear();
            delete p;
            return;
        }
        p->clear();
        mObjects.push_back(p);
    }

private:
    std::vector<T*> mObjects;

    PURE_DISABLE_COPY(ObjectCache)
};

}  // namespace PureCore
