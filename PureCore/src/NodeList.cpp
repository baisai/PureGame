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

#include "PureCore/CoreErrorDesc.h"
#include "PureCore/NodeList.h"

namespace PureCore {
///////////////////////////////////////////////////////////////////////////
// Node
//////////////////////////////////////////////////////////////////////////
Node::Node() : mPre(nullptr), mNext(nullptr) {}

Node::~Node() { leave(); }

void Node::leave() {
    if (mPre != nullptr) {
        mPre->next(mNext);
    }
    if (mNext != nullptr) {
        mNext->pre(mPre);
    }
    mPre = nullptr;
    mNext = nullptr;
}

Node* Node::pre() const { return mPre; }

void Node::pre(Node* pre) { mPre = pre; }

Node* Node::next() const { return mNext; }

void Node::next(Node* next) { mNext = next; }

///////////////////////////////////////////////////////////////////////////
// NodeIter
//////////////////////////////////////////////////////////////////////////
NodeIter::NodeIter(Node* node) : mNode(node) {}

NodeIter& NodeIter::operator++() {
    mNode = mNode->next();
    return *this;
}

NodeIter NodeIter::operator++(int) {
    NodeIter old(mNode);
    mNode = mNode->next();
    return old;
}

NodeIter& NodeIter::operator--() {
    mNode = mNode->pre();
    return *this;
}

NodeIter NodeIter::operator--(int) {
    NodeIter old(mNode);
    mNode = mNode->pre();
    return old;
}

Node& NodeIter::operator*() const { return *(this->mNode); }

Node* NodeIter::operator->() const { return mNode; }

bool NodeIter::operator==(const NodeIter& right) const { return mNode == right.mNode; }

bool NodeIter::operator!=(const NodeIter& right) const { return mNode != right.mNode; }

///////////////////////////////////////////////////////////////////////////
// NodeReverseIter
//////////////////////////////////////////////////////////////////////////
NodeReverseIter::NodeReverseIter(Node* node) : mNode(node) {}

NodeReverseIter& NodeReverseIter::operator++() {
    mNode = mNode->pre();
    return *this;
}

NodeReverseIter NodeReverseIter::operator++(int) {
    NodeReverseIter old(mNode);
    mNode = mNode->pre();
    return old;
}

NodeReverseIter& NodeReverseIter::operator--() {
    mNode = mNode->next();
    return *this;
}

NodeReverseIter NodeReverseIter::operator--(int) {
    NodeReverseIter old(mNode);
    mNode = mNode->next();
    return old;
}

Node& NodeReverseIter::operator*() const { return *(this->mNode); }

Node* NodeReverseIter::operator->() const { return mNode; }

bool NodeReverseIter::operator==(const NodeReverseIter& right) const { return mNode == right.mNode; }

bool NodeReverseIter::operator!=(const NodeReverseIter& right) const { return mNode != right.mNode; }

///////////////////////////////////////////////////////////////////////////
// NodeList
//////////////////////////////////////////////////////////////////////////
NodeList::NodeList() {
    mHead.next(&mTail);
    mTail.pre(&mHead);
}

NodeList::~NodeList() {}

void NodeList::clear() {
    mHead.leave();
    mTail.leave();
    mHead.next(&mTail);
    mTail.pre(&mHead);
}

bool NodeList::empty() const { return mHead.next() == &mTail; }

NodeIter NodeList::begin() const { return NodeIter(mHead.next()); }

NodeIter NodeList::end() const { return NodeIter(const_cast<Node*>(&mTail)); }

NodeReverseIter NodeList::rbegin() const { return NodeReverseIter(mTail.pre()); }

NodeReverseIter NodeList::rend() const { return NodeReverseIter(const_cast<Node*>(&mHead)); }

void NodeList::swap(NodeList& list) {
    NodeList temp;
    temp.push_back_list(list);
    list.push_back_list(*this);
    push_back_list(temp);
}

Node* NodeList::get_front() const {
    if (empty()) {
        return nullptr;
    }
    return mHead.next();
}

Node* NodeList::pop_front() {
    Node* node = get_front();
    if (node == nullptr) {
        return nullptr;
    }
    node->leave();
    return node;
}

Node* NodeList::get_back() const {
    if (empty()) {
        return nullptr;
    }
    return mTail.pre();
}

Node* NodeList::pop_back() {
    Node* node = get_back();
    if (node == nullptr) {
        return nullptr;
    }
    node->leave();
    return node;
}

void NodeList::push_front(Node* node) {
    if (node == nullptr) {
        return;
    }
    node->next(mHead.next());
    node->pre(&mHead);
    mHead.next()->pre(node);
    mHead.next(node);
}

void NodeList::push_back(Node* node) {
    if (node == nullptr) {
        return;
    }
    node->next(&mTail);
    node->pre(mTail.pre());
    mTail.pre()->next(node);
    mTail.pre(node);
}

void NodeList::push_front_list(NodeList& list) {
    if (list.empty()) {
        return;
    }

    Node* listHead = list.mHead.next();
    Node* listTail = list.mTail.pre();
    list.clear();
    if (empty()) {
        mHead.next(listHead);
        mHead.next()->pre(&mHead);
        mTail.pre(listTail);
        mTail.pre()->next(&mTail);
        return;
    }
    mHead.next()->pre(listTail);
    listTail->next(mHead.next());
    mHead.next(listHead);
    listHead->pre(&mHead);
}

void NodeList::push_back_list(NodeList& list) {
    if (list.empty()) {
        return;
    }
    Node* listHead = list.mHead.next();
    Node* listTail = list.mTail.pre();
    list.clear();
    if (empty()) {
        mHead.next(listHead);
        mHead.next()->pre(&mHead);
        mTail.pre(listTail);
        mTail.pre()->next(&mTail);
        return;
    }
    mTail.pre()->next(listHead);
    listHead->pre(mTail.pre());
    mTail.pre(listTail);
    listTail->next(&mTail);
}

}  // namespace PureCore