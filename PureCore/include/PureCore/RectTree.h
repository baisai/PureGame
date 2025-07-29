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
#include "PureCore/Memory/ObjectPool.h"
#include "PureCore/Geometry.h"

#include <vector>
#include <unordered_map>
#include <functional>

namespace PureCore {
class RectTree;
class PURECORE_API RectNode {
public:
    friend class RectTree;
    RectNode(RectTree* tree);
    ~RectNode();

    const IntABox& abox() const;
    int64_t obj_id() const;

private:
    void set_obj(int64_t objID, const IntABox& box);
    void check_objs(std::vector<RectNode*>& result, size_t limit, const std::function<bool(const IntABox& dst)>& check);
    RectNode* choose_subtree(const IntABox& box, int32_t depth, std::vector<RectNode*>& path);
    RectNode* get_child(size_t idx);
    void calc_abox();
    IntABox dist_abox(size_t s, size_t e);
    void extend(const IntABox& box);
    void choose_split_axis(int64_t minElem);
    int64_t dist_margin(int64_t minElem, std::function<int32_t(const IntABox&, const IntABox&)> compare);
    int64_t choose_split_index(int64_t minElem);
    int split_children(int64_t index, int64_t count, RectNode* to);

private:
    RectTree* mTree;
    std::vector<RectNode*> mChildren;
    bool mLeaf = true;
    int32_t mDepth = 0;
    int64_t mObjID = 0;
    IntABox mABox{};
};

class PURECORE_API RectTree {
public:
    friend class RectNode;
    RectTree(uint32_t maxElem = 16);
    ~RectTree();

    void clear();

    const std::vector<RectNode*>& all_objs();
    const std::vector<RectNode*>& collide_abox(const IntABox& box);
    const std::vector<RectNode*>& collide_circle(const IntCircle& cir);
    const std::vector<RectNode*>& collide_sector(const IntSector& sec);
    const std::vector<RectNode*>& collide_obox(const IntOBox& obox);
    bool is_collide_abox(const IntABox& box);
    bool is_collide_circle(const IntCircle& cir);
    bool is_collide_sector(const IntSector& sec);
    bool is_collide_obox(const IntOBox& obox);
    int insert(int64_t objID, const IntABox& box);
    int remove(int64_t objID);

private:
    const std::vector<RectNode*>& collide(const IntABox& box, size_t limit, const std::function<bool(const IntABox& dst)> check = nullptr);

private:
    std::vector<RectNode*>& use_cache();
    void unuse_cache();

    RectNode* get_node();
    void free_node(RectNode* node);

    void split(const std::vector<RectNode*>& insertPath, int32_t depth);
    int remove_node(RectNode* item);
    void merge_node(const std::vector<RectNode*>& path);

private:
    RectNode* mRoot = nullptr;
    uint32_t mMaxElem;
    uint32_t mMinElem;
    std::unordered_map<int64_t, RectNode*> mObjs;
    ObjectPool<RectNode, 128> mPool;
    std::vector<RectNode*> mResult;
    std::vector<std::vector<RectNode*>> mCache;
    size_t mCacheNext = 0;
    std::vector<int64_t> mCacheIndexes;

    PURE_DISABLE_COPY(RectTree)
};
}  // namespace PureCore
