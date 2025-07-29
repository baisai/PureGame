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

#include "PureCore/QuadTree.h"
#include "PureCore/CoreErrorDesc.h"

#include <algorithm>
#include <unordered_set>

namespace PureCore {
///////////////////////////////////////////////////////////////////////////
// QuadNode
//////////////////////////////////////////////////////////////////////////
QuadNode::QuadNode(QuadTree* tree) : mTree(tree) {}

QuadNode::~QuadNode() {
    for (auto c : mChildren) {
        mTree->free_node(c);
    }
    mChildren = {};
    mObjs.clear();
}

void QuadNode::add_obj(QuadObject* obj) {
    if (obj == nullptr) {
        return;
    }
    if (is_leaf()) {
        if (mObjs.size() < mTree->mMaxElem) {
            mObjs.push_back(obj);
        } else {
            split();
            add_obj(obj);
        }
    } else {
        uint8_t idx = quadrant(obj->mABox);
        for (uint8_t i = 0; i < mChildren.size(); ++i) {
            if ((idx & (uint8_t(1) << i)) != 0) {
                mChildren[i]->add_obj(obj);
            }
        }
    }
}

void QuadNode::remove_obj(QuadNode* parent, QuadObject* obj) {
    if (obj == nullptr) {
        return;
    }

    if (is_leaf()) {
        for (size_t i = 0; i < mObjs.size(); ++i) {
            if (mObjs[i]->mObjID == obj->mObjID) {
                mObjs[i] = mObjs.back();
                mObjs.pop_back();
                break;
            }
        }
        if (parent != nullptr && mObjs.empty()) {  // try merge
            parent->try_merge();
        }
    } else {
        uint8_t idx = quadrant(obj->mABox);
        for (uint8_t i = 0; i < mChildren.size(); ++i) {
            if ((idx & (uint8_t(1) << i)) != 0) {
                mChildren[i]->remove_obj(this, obj);
            }
        }
    }
}

void QuadNode::try_merge() {
    if (is_leaf()) {
        return;
    }
    int32_t count = 0;
    for (auto c : mChildren) {
        if (!c->is_leaf()) {
            return;
        }
        count += int32_t(c->mObjs.size());
    }
    if (count == 0) {
        for (auto c : mChildren) {
            mTree->free_node(c);
        }
        mChildren = {};
    }
}

void QuadNode::split() {
    if (!is_leaf()) {
        return;
    }
    IntABox boxes[4]{};
    ArrayRef<IntABox> arr(boxes, 4);
    quadrant_box(arr);
    for (size_t i = 0; i < mChildren.size(); ++i) {
        mChildren[i] = mTree->get_node();
        mChildren[i]->mABox = boxes[i];
    }
    for (auto o : mObjs) {
        uint8_t idx = quadrant(o->mABox);
        for (uint8_t i = 0; i < mChildren.size(); ++i) {
            if ((idx & (uint8_t(1) << i)) != 0) {
                mChildren[i]->mObjs.push_back(o);
            }
        }
    }
    mObjs.clear();
}
int QuadNode::quadrant_box(ArrayRef<IntABox>& arr) const {
    if (arr.size() != 4) {
        return ErrorInvalidArg;
    }
    auto halfSize = mABox.half_size();
    arr[0] = {mABox.mMin.mX + halfSize.mX, mABox.mMin.mY + halfSize.mY, mABox.mMax};
    arr[1] = {mABox.mMin.mX, mABox.mMin.mY + halfSize.mY, mABox.mMin.mX + halfSize.mX, mABox.mMax.mY};
    arr[2] = {mABox.mMin, mABox.mMin.mX + halfSize.mX, mABox.mMin.mY + halfSize.mY};
    arr[3] = {mABox.mMin.mX + halfSize.mX, mABox.mMin.mY, mABox.mMax.mX, mABox.mMin.mY + halfSize.mY};
    return Success;
}

uint8_t QuadNode::quadrant(const IntABox& box) const {
    IntABox boxes[4]{};
    ArrayRef<IntABox> arr(boxes, 4);
    quadrant_box(arr);
    uint8_t flag = 0u;
    for (uint8_t i = 0; i < PURE_ARRAY_SIZE(boxes); ++i) {
        if (boxes[i].intersect(box)) {
            flag |= (uint8_t(1) << i);
        }
    }
    return flag;
}

const IntABox& QuadNode::abox() const { return mABox; }

bool QuadNode::is_leaf() const { return mChildren[0] == nullptr; }

void QuadNode::get_all_objs(std::vector<QuadObject*>& result) {
    auto& searchCache = mTree->use_cache();
    QuadNode* node = this;
    while (node != nullptr) {
        for (auto o : node->mObjs) {
            result.push_back(o);
        }
        for (auto c : node->mChildren) {
            searchCache.push_back(c);
        }
        if (searchCache.empty()) {
            node = nullptr;
        } else {
            node = searchCache.back();
            searchCache.pop_back();
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// QuadTree
//////////////////////////////////////////////////////////////////////////
QuadTree::QuadTree(const IntABox& box, uint32_t maxElem) { reset(box, maxElem); }

QuadTree::~QuadTree() {
    mResult.clear();
    mCache.clear();
    free_node(mRoot);
    mRoot = nullptr;
}

void QuadTree::reset(const IntABox& box, uint32_t maxElem) {
    if (mRoot != nullptr) {
        free_node(mRoot);
        mRoot = nullptr;
    }
    for (auto const& o : mObjs) {
        free_obj(o.second);
    }
    mObjs.clear();
    mCache.clear();
    mResult.clear();
    mMaxElem = std::max(uint32_t(4), maxElem);
    mRoot = get_node();
    mRoot->mABox = box;
}

const std::vector<QuadObject*>& QuadTree::all_objs() {
    mResult.clear();
    mRoot->get_all_objs(mResult);
    return mResult;
}

const std::vector<QuadObject*>& QuadTree::collide_abox(const IntABox& box) { return collide(box, 0); }

const std::vector<QuadObject*>& QuadTree::collide_circle(const IntCircle& cir) {
    auto box = cir.get_bounding();
    return collide(box, 0, [&cir](const IntABox& dst) { return dst.intersect_circle(cir); });
}

const std::vector<QuadObject*>& QuadTree::collide_sector(const IntSector& sec) {
    auto box = sec.get_bounding();
    return collide(box, 0, [&sec](const IntABox& dst) { return dst.intersect_sector(sec); });
}

const std::vector<QuadObject*>& QuadTree::collide_obox(const IntOBox& obox) {
    auto box = obox.get_bounding();
    return collide(box, 0, [&obox](const IntABox& dst) { return dst.intersect_obox(obox); });
}

bool QuadTree::is_collide_abox(const IntABox& box) { return !collide(box, 1).empty(); }

bool QuadTree::is_collide_circle(const IntCircle& cir) {
    auto box = cir.get_bounding();
    return !collide(box, 1, [&cir](const IntABox& dst) { return dst.intersect_circle(cir); }).empty();
}

bool QuadTree::is_collide_sector(const IntSector& sec) {
    auto box = sec.get_bounding();
    return !collide(box, 1, [&sec](const IntABox& dst) { return dst.intersect_sector(sec); }).empty();
}

bool QuadTree::is_collide_obox(const IntOBox& obox) {
    auto box = obox.get_bounding();
    return !collide(box, 1, [&obox](const IntABox& dst) { return dst.intersect_obox(obox); }).empty();
}

int QuadTree::insert(int64_t objID, const IntABox& box) {
    if (objID <= 0) {
        return ErrorInvalidArg;
    }
    auto iter = mObjs.find(objID);
    if (iter != mObjs.end()) {
        mRoot->remove_obj(nullptr, iter->second);
        free_obj(iter->second);
        mObjs.erase(iter);
    }

    auto obj = get_obj();
    obj->mObjID = objID;
    obj->mABox = box;
    mObjs.insert(std::make_pair(objID, obj));
    mRoot->add_obj(obj);
    return Success;
}

int QuadTree::remove(int64_t objID) {
    if (objID <= 0) {
        return ErrorInvalidArg;
    }
    auto iter = mObjs.find(objID);
    if (iter != mObjs.end()) {
        mRoot->remove_obj(nullptr, iter->second);
        free_obj(iter->second);
        mObjs.erase(iter);
    }
    return Success;
}

const std::vector<QuadObject*>& QuadTree::collide(const IntABox& box, size_t limit, const std::function<bool(const IntABox& dst)> check) {
    mResult.clear();
    QuadNode* node = mRoot;
    if (!box.intersect(node->abox())) {
        return mResult;
    }
    std::unordered_set<int64_t> ids;
    auto& searchCache = use_cache();
    while (node != nullptr) {
        for (auto o : node->mObjs) {
            if (o->mABox.intersect(box) && ids.find(o->mObjID) == ids.end()) {
                if (check && !check(o->mABox)) {
                    continue;
                }
                mResult.push_back(o);
                ids.insert(o->mObjID);
                if (limit != 0 && mResult.size() >= limit) {
                    return mResult;
                }
            }
        }
        if (!node->is_leaf()) {
            for (auto c : node->mChildren) {
                if (box.intersect(c->mABox)) {
                    searchCache.push_back(c);
                }
            }
        }

        if (searchCache.empty()) {
            node = nullptr;
        } else {
            node = searchCache.back();
            searchCache.pop_back();
        }
    }
    return mResult;
}

std::vector<QuadNode*>& QuadTree::use_cache() { return mCache; }

QuadNode* QuadTree::get_node() { return mNodePool.get(this); }

void QuadTree::free_node(QuadNode* node) { mNodePool.free(node); }

QuadObject* QuadTree::get_obj() { return mObjPool.get(); }

void QuadTree::free_obj(QuadObject* obj) { return mObjPool.free(obj); }

}  // namespace PureCore