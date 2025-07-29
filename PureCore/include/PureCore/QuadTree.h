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

#include <array>
#include <unordered_map>
#include <functional>

namespace PureCore {
struct PURECORE_API QuadObject {
    int64_t mObjID = 0;
    IntABox mABox{};
};

class QuadTree;
class QuadNode {
public:
    friend class QuadTree;
    QuadNode(QuadTree* tree);
    ~QuadNode();

private:
    void add_obj(QuadObject* obj);
    void remove_obj(QuadNode* parent, QuadObject* obj);
    void try_merge();
    void split();
    int quadrant_box(ArrayRef<IntABox>& arr) const;
    uint8_t quadrant(const IntABox& box) const;
    const IntABox& abox() const;
    bool is_leaf() const;
    void get_all_objs(std::vector<QuadObject*>& result);

private:
    QuadTree* mTree;
    std::array<QuadNode*, 4> mChildren{};
    std::vector<QuadObject*> mObjs;
    IntABox mABox{};
};

class PURECORE_API QuadTree {
public:
    friend class QuadNode;
    QuadTree(const IntABox& box, uint32_t maxElem = 16);
    ~QuadTree();

    void reset(const IntABox& box, uint32_t maxElem = 16);

    const std::vector<QuadObject*>& all_objs();
    const std::vector<QuadObject*>& collide_abox(const IntABox& box);
    const std::vector<QuadObject*>& collide_circle(const IntCircle& cir);
    const std::vector<QuadObject*>& collide_sector(const IntSector& sec);
    const std::vector<QuadObject*>& collide_obox(const IntOBox& obox);
    bool is_collide_abox(const IntABox& box);
    bool is_collide_circle(const IntCircle& cir);
    bool is_collide_sector(const IntSector& sec);
    bool is_collide_obox(const IntOBox& obox);
    int insert(int64_t objID, const IntABox& box);
    int remove(int64_t objID);

private:
    const std::vector<QuadObject*>& collide(const IntABox& box, size_t limit, const std::function<bool(const IntABox& dst)> check = nullptr);

private:
    std::vector<QuadNode*>& use_cache();

    QuadNode* get_node();
    void free_node(QuadNode* node);
    QuadObject* get_obj();
    void free_obj(QuadObject* obj);

private:
    QuadNode* mRoot = nullptr;
    uint32_t mMaxElem;
    std::unordered_map<int64_t, QuadObject*> mObjs;
    ObjectPool<QuadNode, 128> mNodePool;
    ObjectPool<QuadObject, 128> mObjPool;
    std::vector<QuadNode*> mCache;
    std::vector<QuadObject*> mResult;

    PURE_DISABLE_COPY(QuadTree)
};
}  // namespace PureCore
