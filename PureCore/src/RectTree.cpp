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

#include "PureCore/RectTree.h"
#include "PureCore/CoreErrorDesc.h"

#include <algorithm>
#include <cmath>

namespace PureCore {
static int32_t compare_bbox_x(const IntABox& l, const IntABox& r) { return l.mMin.mX - r.mMin.mX; }
static int32_t compare_bbox_y(const IntABox& l, const IntABox& r) { return l.mMin.mY - r.mMin.mY; }
///////////////////////////////////////////////////////////////////////////
// RectNode
//////////////////////////////////////////////////////////////////////////
RectNode::RectNode(RectTree* tree) : mTree(tree) { mChildren.reserve(tree->mMaxElem); }

RectNode::~RectNode() {
    for (auto c : mChildren) {
        mTree->free_node(c);
    }
    mChildren.clear();
}

const IntABox& RectNode::abox() const { return mABox; }

int64_t RectNode::obj_id() const { return mObjID; }

void RectNode::set_obj(int64_t objID, const IntABox& box) {
    mObjID = objID;
    mABox = box;
}

void RectNode::check_objs(std::vector<RectNode*>& result, size_t limit, const std::function<bool(const IntABox& dst)>& check) {
    auto& searchCache = mTree->use_cache();
    RectNode* node = this;
    while (node != nullptr) {
        if (node->mLeaf) {
            for (auto c : node->mChildren) {
                if (check && !check(c->mABox)) {
                    continue;
                }
                result.push_back(c);
                if (limit != 0 && result.size() >= limit) {
                    mTree->unuse_cache();
                    return;
                }
            }
        } else {
            for (auto c : node->mChildren) {
                searchCache.push_back(c);
            }
        }
        if (searchCache.empty()) {
            node = nullptr;
        } else {
            node = searchCache.back();
            searchCache.pop_back();
        }
    }
    mTree->unuse_cache();
}

RectNode* RectNode::choose_subtree(const IntABox& box, int32_t depth, std::vector<RectNode*>& path) {
    RectNode* node = this;
    while (node != nullptr) {
        path.push_back(node);
        if (node->mLeaf || path.size() - 1 == depth) {
            break;
        }
        int64_t minArea = INT64_MAX;
        int64_t minEnlarge = INT64_MAX;
        RectNode* target = nullptr;
        for (auto c : node->mChildren) {
            int64_t area = c->abox().box_area();
            int64_t enlarge = box.enlarge_area(c->abox()) - area;
            // chose large area
            if (enlarge < minEnlarge) {
                minEnlarge = enlarge;
                minArea = area < minArea ? area : minArea;
                target = c;
            } else if (enlarge == minEnlarge) {
                if (area < minArea) {
                    minArea = area;
                    target = c;
                }
            }
        }
        if (target != nullptr) {
            node = target;
        } else {
            node = node->get_child(0);
        }
    }
    return node;
}

RectNode* RectNode::get_child(size_t idx) {
    if (idx < mChildren.size()) {
        return mChildren[idx];
    }
    return nullptr;
}

void RectNode::calc_abox() { mABox = dist_abox(0, mChildren.size()); }

IntABox RectNode::dist_abox(size_t s, size_t e) {
    IntABox box{INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN};
    for (size_t i = s; i < e && i < mChildren.size(); ++i) {
        box.extend(mChildren[i]->abox());
    }
    return box;
}

void RectNode::extend(const IntABox& box) { mABox.extend(box); }

void RectNode::choose_split_axis(int64_t minElem) {
    auto xMargin = dist_margin(minElem, compare_bbox_x);
    auto yMargin = dist_margin(minElem, compare_bbox_y);
    if (xMargin < yMargin) {
        std::sort(mChildren.begin(), mChildren.end(), [](const RectNode* a, const RectNode* b) { return compare_bbox_x(a->abox(), b->abox()) < 0; });
    }
}

int64_t RectNode::dist_margin(int64_t minElem, std::function<int32_t(const IntABox&, const IntABox&)> compare) {
    std::sort(mChildren.begin(), mChildren.end(), [compare](const RectNode* a, const RectNode* b) { return compare(a->abox(), b->abox()) < 0; });
    int64_t countElem = mChildren.size();
    auto leftBox = dist_abox(0, minElem);
    auto rightBox = dist_abox(minElem, countElem - minElem);
    int64_t margin = leftBox.box_margin() + rightBox.box_margin();
    for (int64_t i = minElem; i < countElem - minElem; ++i) {
        leftBox.extend(mChildren[i]->abox());
        margin += leftBox.box_margin();
    }
    for (int64_t i = countElem - minElem - 1; i >= minElem && i < countElem; --i) {
        rightBox.extend(mChildren[i]->abox());
        margin += rightBox.box_margin();
    }
    return margin;
}

int64_t RectNode::choose_split_index(int64_t minElem) {
    int64_t countElem = mChildren.size();
    int64_t index = -1;
    int64_t minOverlap = INT64_MAX;
    int64_t minArea = INT64_MAX;
    for (int64_t i = minElem; i <= countElem - minElem; ++i) {
        auto box1 = dist_abox(0, i);
        auto box2 = dist_abox(i, countElem);
        auto overlap = box1.intersect_area(box2);
        auto area = box1.box_area() + box2.box_area();
        if (overlap < minOverlap) {
            minOverlap = overlap;
            index = i;
            minArea = area < minArea ? area : minArea;
        } else if (overlap == minOverlap) {
            if (area < minArea) {
                minArea = area;
                index = i;
            }
        }
    }
    if (index < 0) {
        index = countElem - minElem;
    }
    return index;
}

int RectNode::split_children(int64_t index, int64_t count, RectNode* to) {
    if (index >= int64_t(mChildren.size()) || count <= 0 || to == nullptr) {
        return ErrorInvalidArg;
    }
    int64_t endIndex = std::min(index + count, int64_t(mChildren.size()));
    for (int64_t i = index; i < endIndex; ++i) {
        to->mChildren.push_back(mChildren[i]);
    }
    mChildren.resize(index);
    return Success;
}

///////////////////////////////////////////////////////////////////////////
// RectTree
//////////////////////////////////////////////////////////////////////////
RectTree::RectTree(uint32_t maxElem) {
    mMaxElem = std::max(uint32_t(4), maxElem);
    mMinElem = std::max(uint32_t(2), uint32_t(std::ceil(0.4 * maxElem)));
    clear();
}

RectTree::~RectTree() {
    mResult.clear();
    mCache.clear();
    free_node(mRoot);
    mRoot = nullptr;
}

void RectTree::clear() {
    if (mRoot != nullptr) {
        free_node(mRoot);
        mRoot = nullptr;
    }
    mRoot = get_node();
    mRoot->mDepth = 1;
    mRoot->mLeaf = true;
    mRoot->calc_abox();
    mObjs.clear();
    mResult.clear();
    mCache.clear();
    mCacheNext = 0;
    mCacheIndexes.clear();
}

const std::vector<RectNode*>& RectTree::all_objs() {
    mResult.clear();
    mRoot->check_objs(mResult, 0, std::function<bool(const IntABox&)>());
    return mResult;
}

const std::vector<RectNode*>& RectTree::collide_abox(const IntABox& box) { return collide(box, 0); }

const std::vector<RectNode*>& RectTree::collide_circle(const IntCircle& cir) {
    auto box = cir.get_bounding();
    return collide(box, 0, [&cir](const IntABox& dst) { return dst.intersect_circle(cir); });
}

const std::vector<RectNode*>& RectTree::collide_sector(const IntSector& sec) {
    auto box = sec.get_bounding();
    return collide(box, 0, [&sec](const IntABox& dst) { return dst.intersect_sector(sec); });
}

const std::vector<RectNode*>& RectTree::collide_obox(const IntOBox& obox) {
    auto box = obox.get_bounding();
    return collide(box, 0, [&obox](const IntABox& dst) { return dst.intersect_obox(obox); });
}

bool RectTree::is_collide_abox(const IntABox& box) { return !collide(box, 1).empty(); }

bool RectTree::is_collide_circle(const IntCircle& cir) {
    auto box = cir.get_bounding();
    return !collide(box, 1, [&cir](const IntABox& dst) { return dst.intersect_circle(cir); }).empty();
}

bool RectTree::is_collide_sector(const IntSector& sec) {
    auto box = sec.get_bounding();
    return !collide(box, 1, [&sec](const IntABox& dst) { return dst.intersect_sector(sec); }).empty();
}

bool RectTree::is_collide_obox(const IntOBox& obox) {
    auto box = obox.get_bounding();
    return !collide(box, 1, [&obox](const IntABox& dst) { return dst.intersect_obox(obox); }).empty();
}

int RectTree::insert(int64_t objID, const IntABox& box) {
    if (objID <= 0) {
        return ErrorInvalidArg;
    }
    auto iter = mObjs.find(objID);
    if (iter != mObjs.end()) {
        mObjs.erase(iter);
        remove_node(iter->second);
    }
    int32_t depth = mRoot->mDepth - 1;
    auto& insertPath = use_cache();

    RectNode* node = mRoot->choose_subtree(box, depth, insertPath);
    auto newNode = get_node();
    newNode->set_obj(objID, box);
    mObjs.insert(std::make_pair(objID, newNode));
    node->mChildren.push_back(newNode);
    node->extend(box);
    while (depth >= 0 && depth < insertPath.size()) {
        if (insertPath[depth]->mChildren.size() > mMaxElem) {
            split(insertPath, depth);
            --depth;
        } else {
            break;
        }
    }
    for (int32_t i = depth; i >= 0 && i < insertPath.size(); --i) {
        insertPath[i]->extend(box);
    }

    unuse_cache();
    return Success;
}

int RectTree::remove(int64_t objID) {
    if (objID <= 0) {
        return ErrorInvalidArg;
    }
    auto iter = mObjs.find(objID);
    if (iter != mObjs.end()) {
        auto node = iter->second;
        mObjs.erase(iter);
        return remove_node(node);
    }
    return Success;
}

const std::vector<RectNode*>& RectTree::collide(const IntABox& box, size_t limit, const std::function<bool(const IntABox& dst)> check) {
    mResult.clear();
    RectNode* node = mRoot;
    if (!box.intersect(node->abox())) {
        return mResult;
    }
    auto& searchCache = use_cache();
    while (node != nullptr) {
        for (auto c : node->mChildren) {
            if (box.intersect(c->mABox)) {
                if (node->mLeaf) {
                    if (check && !check(c->mABox)) {
                        continue;
                    }
                    mResult.push_back(c);
                } else if (box.contain(c->abox())) {
                    c->check_objs(mResult, limit, check);
                } else {
                    searchCache.push_back(c);
                }
                if (limit != 0 && mResult.size() >= limit) {
                    unuse_cache();
                    return mResult;
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
    unuse_cache();
    return mResult;
}

std::vector<RectNode*>& RectTree::use_cache() {
    while (mCacheNext >= mCache.size()) {
        mCache.emplace_back();
    }
    return mCache[mCacheNext++];
}

void RectTree::unuse_cache() {
    if (mCacheNext >= mCache.size()) {
        return;
    }
    mCache[mCacheNext--].clear();
}

RectNode* RectTree::get_node() { return mPool.get(this); }

void RectTree::free_node(RectNode* node) { mPool.free(node); }

void RectTree::split(const std::vector<RectNode*>& insertPath, int32_t depth) {
    auto node = insertPath[depth];
    node->choose_split_axis(mMinElem);
    auto splitIndex = node->choose_split_index(mMinElem);
    auto newNode = get_node();
    newNode->mDepth = node->mDepth;
    newNode->mLeaf = node->mLeaf;
    node->split_children(splitIndex, int64_t(node->mChildren.size()) - splitIndex, newNode);
    node->calc_abox();
    newNode->calc_abox();
    if (depth > 0) {
        insertPath[depth - 1]->mChildren.push_back(newNode);
    } else {
        // split root
        auto newRoot = get_node();
        newRoot->mDepth = node->mDepth + 1;
        newRoot->mLeaf = false;
        newRoot->mChildren.push_back(node);
        newRoot->mChildren.push_back(newNode);
        mRoot = newRoot;
        mRoot->calc_abox();
    }
}

int RectTree::remove_node(RectNode* item) {
    if (item == nullptr) {
        return ErrorInvalidArg;
    }

    auto node = mRoot;
    auto& path = use_cache();
    auto& indexes = mCacheIndexes;
    indexes.clear();
    RectNode* parent = nullptr;
    bool goingUp = false;
    int64_t i = 0;
    while (node != nullptr || !path.empty()) {
        if (node == nullptr) {  // up
            node = path.back();
            path.pop_back();
            parent = path.back();
            i = indexes.back();
            indexes.pop_back();
            goingUp = true;
        }
        if (node->mLeaf) {  // current
            for (size_t i = 0; i < node->mChildren.size(); ++i) {
                if (node->mChildren[i]->mObjID == item->mObjID) {
                    // found and remove
                    free_node(node->mChildren[i]);
                    node->mChildren[i] = node->mChildren.back();
                    node->mChildren.pop_back();
                    // merge
                    path.push_back(node);
                    merge_node(path);
                    indexes.clear();
                    unuse_cache();
                    return Success;
                }
            }
        }
        if (!goingUp && !node->mLeaf && node->abox().contain(item->abox())) {  // down
            path.push_back(node);
            indexes.push_back(i);
            i = 0;
            parent = node;
            if (node->mChildren.empty()) {
                node = nullptr;
            } else {
                node = node->mChildren[0];
            }
        } else if (parent != nullptr) {  // right
            i++;
            if (i >= int64_t(parent->mChildren.size())) {
                node = nullptr;
            } else {
                node = parent->mChildren[i];
            }
            goingUp = false;
        } else {
            node = nullptr;
        }
    }
    indexes.clear();
    unuse_cache();
    return Success;
}

void RectTree::merge_node(const std::vector<RectNode*>& path) {
    if (path.empty()) {
        return;
    }
    for (int64_t i = path.size() - 1; i >= 0; --i) {
        if (path[i]->mChildren.empty()) {
            if (i > 0) {
                auto& children = path[i - 1]->mChildren;
                for (size_t j = 0; j < children.size(); ++j) {
                    if (children[j] == path[i]) {
                        free_node(children[j]);
                        children[j] = children.back();
                        children.pop_back();
                        break;
                    }
                }
            } else {
                clear();
            }
        } else {
            path[i]->calc_abox();
        }
    }
}

}  // namespace PureCore