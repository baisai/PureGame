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

#include "PureNet/PureNetLib.h"
#include "PureNet/Link.h"

#include <iostream>

namespace PureNet {
struct PURENET_API LinkAllocator {
    LinkAllocator(std::function<Link*()>&& allocator, std::function<void(Link*)>&& deallocator);
    std::function<Link*()> mAllocator;
    std::function<void(Link*)> mDeallocator;
};
class PURENET_API LinkFactory {
public:
    static Link* get(LinkType key);

    template <typename T>
    static Link* get();

    template <typename T>
    static LinkType key();

private:
    template <typename T>
    friend class LinkFactoryAdapter;
    static std::unordered_map<LinkType, LinkAllocator>& get_factory();
};

template <typename T>
class LinkFactoryAdapter {
private:
    template <typename LT>
    friend class LinkPool;
    LinkFactoryAdapter();
};

template <typename T>
class LinkPool {
private:
    friend class LinkFactory;
    template <typename LT>
    friend class LinkFactoryAdapter;

    static LinkType key();
    static Link* get();
    static void free(Link* link);
    static thread_local PureCore::ObjectCache<T, 256> tlPool;
    static const uint8_t sKey;
    static const LinkFactoryAdapter<T> __;
};

template <typename T>
Link* LinkFactory::get() {
    return get(key<T>());
}

template <typename T>
LinkType LinkFactory::key() {
    return LinkPool<T>::key();
}

template <typename T>
LinkFactoryAdapter<T>::LinkFactoryAdapter() {
    if (!LinkFactory::get_factory().insert(std::make_pair(LinkPool<T>::key(), LinkAllocator(LinkPool<T>::get, LinkPool<T>::free))).second) {
        std::cerr << typeid(T).name() << " already adapter link factory" << std::endl;
    }
}

template <typename T>
LinkType LinkPool<T>::key() {
    (void)__;
    return (LinkType)&sKey;
}

template <typename T>
Link* LinkPool<T>::get() {
    return tlPool.get();
}

template <typename T>
void LinkPool<T>::free(Link* link) {
    tlPool.free(static_cast<T*>(link));
}

template <typename T>
thread_local PureCore::ObjectCache<T, 256> LinkPool<T>::tlPool{};
template <typename T>
const uint8_t LinkPool<T>::sKey{};
template <typename T>
const LinkFactoryAdapter<T> LinkPool<T>::__{};

}  // namespace PureNet
