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
#include "PureCore/ArrayRef.h"

#include <vector>
#include <cstdint>

namespace PureCore {
struct IntLine;
struct IntABox;
struct PURECORE_API IntPoint {
    IntPoint sub(const IntPoint& p) const;
    IntPoint add(const IntPoint& p) const;
    IntPoint mul(double r) const;
    IntPoint div(double r) const;
    double dist() const;
    int64_t dist2() const;
    int64_t dot(const IntPoint& p) const;
    int64_t cross(const IntPoint& p) const;
    IntPoint projection(const IntLine& line) const;
    IntABox get_bounding() const;

    int32_t mX = 0;
    int32_t mY = 0;
};

struct PURECORE_API IntLine {
    bool get_intersect_point(const IntLine& l, IntPoint& p) const;
    bool intersect(const IntLine& l) const;
    IntABox get_bounding() const;

    IntPoint mMin;
    IntPoint mMax;
};

struct PURECORE_API IntCircle {
    int32_t get_intersect_point(const IntLine& line, ArrayRef<IntPoint>& p) const;
    IntABox get_bounding() const;

    IntPoint mCenter;
    int32_t mRadius;
};

struct PURECORE_API IntSector {
    bool angle_point(const IntPoint& p) const;
    bool contain_point(const IntPoint& p) const;
    IntPoint get_min_angle_point() const;
    IntPoint get_max_angle_point() const;
    IntABox get_bounding() const;

    IntPoint mCenter;
    int32_t mRadius = 0;
    double mMinAngle = 0.0;
    double mMaxAngle = 0.0;
};

struct IntOBox;
struct PURECORE_API IntABox {
    bool intersect(const IntABox& box) const;
    bool contain(const IntABox& box) const;
    bool contain_point(const IntPoint& p) const;
    bool intersect_obox(const IntOBox& obox) const;
    bool intersect_line(const IntLine& l) const;
    bool intersect_circle(const IntCircle& cc) const;
    bool intersect_sector(const IntSector& sec) const;

    int64_t enlarge_area(const IntABox& box) const;
    int64_t box_area() const;
    int64_t box_margin() const;
    int64_t intersect_area(const IntABox& box) const;
    void extend(const IntABox& box);
    IntOBox to_obox() const;
    IntPoint center() const;
    IntPoint size() const;
    IntPoint half_size() const;

    IntPoint mMin;
    IntPoint mMax;
};

struct PURECORE_API IntOBox {
    int get_vertices(ArrayRef<IntPoint>& points) const;
    bool inersect(const IntOBox& box) const;
    IntABox get_bounding() const;

    IntPoint mCenter;
    IntPoint mHalfSize;
    double mAngle = 0.0;
};
}  // namespace PureCore
