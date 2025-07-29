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

#include <iterator>

namespace PureCore {
// sub class must in destroy function call leave()
class PURECORE_API Node {
public:
    Node();
    virtual ~Node();

    void leave();

    template <typename T>
    inline const T* cast() const {
        return static_cast<const T*>(this);
    }

    template <typename T>
    inline T* cast() {
        return static_cast<T*>(this);
    }

private:
    friend class NodeIter;
    friend class NodeReverseIter;
    friend class NodeList;
    Node* pre() const;
    void pre(Node* pre);
    Node* next() const;
    void next(Node* next);

private:
    Node* mPre;
    Node* mNext;

    PURE_DISABLE_COPY(Node)
};

class PURECORE_API NodeIter {
public:
    typedef Node& reference;
    typedef Node* pointer;
    typedef Node value_type;
    typedef ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    NodeIter() = default;
    explicit NodeIter(Node* node);
    ~NodeIter() = default;

    NodeIter& operator++();
    NodeIter operator++(int);
    NodeIter& operator--();
    NodeIter operator--(int);

    Node& operator*() const;
    Node* operator->() const;

    bool operator==(const NodeIter& right) const;
    bool operator!=(const NodeIter& right) const;

private:
    Node* mNode = nullptr;
};

class PURECORE_API NodeReverseIter {
public:
    typedef Node& reference;
    typedef Node* pointer;
    typedef Node value_type;
    typedef ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    NodeReverseIter() = default;
    explicit NodeReverseIter(Node* node);
    ~NodeReverseIter() = default;

    NodeReverseIter& operator++();
    NodeReverseIter operator++(int);
    NodeReverseIter& operator--();
    NodeReverseIter operator--(int);

    Node& operator*() const;
    Node* operator->() const;

    bool operator==(const NodeReverseIter& right) const;
    bool operator!=(const NodeReverseIter& right) const;

private:
    Node* mNode = nullptr;
};

class PURECORE_API NodeList {
public:
    NodeList();
    ~NodeList();

    void clear();

    bool empty() const;

    NodeIter begin() const;
    NodeIter end() const;
    NodeReverseIter rbegin() const;
    NodeReverseIter rend() const;

    void swap(NodeList& list);

    Node* get_front() const;
    Node* pop_front();
    Node* get_back() const;
    Node* pop_back();
    void push_front(Node* node);
    void push_back(Node* node);
    void push_front_list(NodeList& list);
    void push_back_list(NodeList& list);

    template <typename T>
    inline T* get_front_t() const {
        return static_cast<T*>(get_front());
    }
    template <typename T>
    inline T* pop_front_t() {
        return static_cast<T*>(pop_front());
    }
    template <typename T>
    inline T* get_back_t() const {
        return static_cast<T*>(get_back());
    }
    template <typename T>
    inline T* pop_back_t() {
        return static_cast<T*>(pop_back());
    }

    PURE_DISABLE_COPY(NodeList)

private:
    Node mHead;
    Node mTail;
};

}  // namespace PureCore
