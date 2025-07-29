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

#include "PureCore/Geometry.h"
#include "PureCore/CoreErrorDesc.h"

#include <algorithm>
#include <cmath>

namespace PureCore {
static int32_t line_is_turn(const IntPoint& p1, const IntPoint& p2, const IntPoint& p3) {
    int64_t a = int64_t(p3.mX - p1.mX) * (p2.mY - p1.mY);
    int64_t b = int64_t(p2.mX - p1.mX) * (p3.mY - p1.mY);
    return a > b ? 1 : (a < b ? -1 : 0);
}
static bool is_separated_axis(const IntPoint& axis, const ArrayRef<IntPoint>& pgA, const ArrayRef<IntPoint>& pgB) {
    if (pgA.empty() || pgB.empty()) {
        return false;
    }
    int64_t minA = axis.dot(pgA[0]);
    int64_t maxA = minA;
    for (size_t i = 1; i < pgA.size(); ++i) {
        int64_t d = axis.dot(pgA[i]);
        minA = std::min(minA, d);
        maxA = std::max(maxA, d);
    }

    int64_t minB = axis.dot(pgB[0]);
    int64_t maxB = minB;
    for (size_t i = 1; i < pgB.size(); ++i) {
        int64_t d = axis.dot(pgB[i]);
        minB = std::min(minB, d);
        maxB = std::max(maxB, d);
    }
    return !(minA <= maxB && maxA >= minB);
}

static IntABox get_vertices_bouding(ArrayRef<IntPoint> vert) {
    if (vert.empty()) {
        return IntABox{};
    }
    IntABox box{INT32_MIN, INT32_MIN, INT32_MAX, INT32_MAX};
    for (size_t i = 1; i < PURE_ARRAY_SIZE(vert); ++i) {
        if (box.mMin.mX > vert[i].mX) {
            box.mMin.mX = vert[i].mX;
        }
        if (box.mMin.mY > vert[i].mY) {
            box.mMin.mY = vert[i].mY;
        }
        if (box.mMax.mX < vert[i].mX) {
            box.mMax.mX = vert[i].mX;
        }
        if (box.mMax.mY < vert[i].mY) {
            box.mMax.mY = vert[i].mY;
        }
    }
    return box;
}

///////////////////////////////////////////////////////////////////////////
// IntPoint
//////////////////////////////////////////////////////////////////////////
IntPoint IntPoint::sub(const IntPoint& p) const { return IntPoint{mX - p.mX, mY - p.mY}; }

IntPoint IntPoint::add(const IntPoint& p) const { return IntPoint{mX + p.mX, mY + p.mY}; }

IntPoint IntPoint::mul(double r) const { return IntPoint{int32_t(mX * r), int32_t(mY * r)}; }

IntPoint IntPoint::div(double r) const { return IntPoint{int32_t(mX / r), int32_t(mY / r)}; }

double IntPoint::dist() const { return sqrt(double(dist2())); }

int64_t IntPoint::dist2() const { return mX * mX + mY * mY; }

int64_t IntPoint::dot(const IntPoint& p) const { return int64_t(mX) * p.mX + int64_t(mY) * p.mY; }

int64_t IntPoint::cross(const IntPoint& p) const { return int64_t(mX) * p.mY - int64_t(mY) * p.mX; }

IntPoint IntPoint::projection(const IntLine& line) const {
    IntPoint base = line.mMax.sub(line.mMin);
    double r = double(sub(line.mMin).dot(base)) / base.dist2();
    auto p = base.mul(r);
    p.mX += line.mMin.mX;
    p.mY += line.mMin.mY;
    return p;
}

IntABox IntPoint::get_bounding() const { return IntABox{mX, mY, mX, mY}; }

///////////////////////////////////////////////////////////////////////////
// IntLine
//////////////////////////////////////////////////////////////////////////
// https://stackoverflow.com/a/24392281/1749528
bool IntLine::get_intersect_point(const IntLine& l, IntPoint& p) const {
    int32_t dX = mMax.mX - mMin.mX;
    int32_t dY = mMax.mY - mMin.mY;
    int64_t deter = dX * int64_t(l.mMax.mY - l.mMin.mY) - int64_t(l.mMax.mX - l.mMin.mX) * dY;
    if (deter == 0) {
        return false;
    }
    double lambda = double((l.mMax.mY - l.mMin.mY) * (l.mMax.mX - mMin.mX) + (l.mMin.mX - l.mMax.mX) * (l.mMax.mY - mMin.mY)) / deter;
    double gamma = double((mMin.mY - mMax.mY) * (l.mMax.mX - mMin.mX) + dX * (l.mMax.mY - mMin.mY)) / deter;
    // check if there is an intersect
    if (!(lambda >= 0 && lambda <= 1) || !(gamma >= 0 && gamma <= 1)) {
        return false;
    }
    p.mX = int32_t(mMin.mX + lambda * dX);
    p.mY = int32_t(mMin.mY + lambda * dY);
    return true;
}

// https://stackoverflow.com/a/16725715/1749528
bool IntLine::intersect(const IntLine& l) const {
    return line_is_turn(mMin, l.mMin, l.mMax) != line_is_turn(mMax, l.mMin, l.mMax) && line_is_turn(mMin, mMax, l.mMin) != line_is_turn(mMin, mMax, l.mMax);
}

IntABox IntLine::get_bounding() const { return IntABox{mMin, mMax}; }

///////////////////////////////////////////////////////////////////////////
// IntCircle
//////////////////////////////////////////////////////////////////////////
// https://stackoverflow.com/a/37225895/1749528
int32_t IntCircle::get_intersect_point(const IntLine& line, ArrayRef<IntPoint>& p) const {
    if (p.size() != 2) {
        return 0;  // must 2 size out result;
    }
    uint32_t count = 0;
    auto v1 = line.mMax.sub(line.mMin);
    auto v2 = line.mMin.sub(mCenter);
    int64_t b = (int64_t(v1.mX) * v2.mX + int64_t(v1.mY) * v2.mY) * -2;
    int64_t c = (int64_t(v1.mX) * v1.mX + int64_t(v1.mY) * v1.mY) * 2;
    int64_t d2 = b * b - (int64_t(v2.mX) * v2.mX + int64_t(v2.mY) * v2.mY - int64_t(mRadius) * mRadius) * c * 2;
    if (d2 < 0) {
        return count;
    }
    double d = sqrt(d2);

    auto u1 = (b - d) / c;  // these represent the unit distance of point one and two on the line
    auto u2 = (b + d) / c;
    if (u1 <= 1 && u1 >= 0) {
        // add point if on the line
        ++count;
        p[0].mX = int32_t(line.mMin.mX + v1.mX * u1);
        p[0].mY = int32_t(line.mMin.mY + v1.mY * u1);
    }
    if (u2 <= 1 && u2 >= 0) {
        // second add point if on the line
        ++count;
        p[1].mX = int32_t(line.mMin.mX + v1.mX * u2);
        p[1].mY = int32_t(line.mMin.mY + v1.mY * u2);
    }
    return count;
}

IntABox IntCircle::get_bounding() const { return IntABox{mCenter.mX - mRadius, mCenter.mY - mRadius, mCenter.mX + mRadius, mCenter.mY + mRadius}; }

///////////////////////////////////////////////////////////////////////////
// IntSector
//////////////////////////////////////////////////////////////////////////
bool IntSector::angle_point(const IntPoint& p) const {
    auto v = p.sub(mCenter);
    auto angle = std::atan2(v.mY, v.mX);
    return angle >= mMinAngle && angle <= mMaxAngle;
}

bool IntSector::contain_point(const IntPoint& p) const {
    if (p.sub(mCenter).dist() > mRadius) {
        return false;
    }
    return angle_point(p);
}

IntPoint IntSector::get_min_angle_point() const {
    return IntPoint{int32_t(std::cos(mMinAngle) * mRadius) + mCenter.mX, int32_t(std::sin(mMinAngle) * mRadius) + mCenter.mY};
}

IntPoint IntSector::get_max_angle_point() const {
    return IntPoint{int32_t(std::cos(mMaxAngle) * mRadius) + mCenter.mX, int32_t(std::sin(mMaxAngle) * mRadius) + mCenter.mY};
}

IntABox IntSector::get_bounding() const {
    IntPoint vert[7]{};
    vert[0] = mCenter;
    vert[1] = get_min_angle_point();
    vert[2] = get_max_angle_point();
    size_t i = 3;
    vert[i] = {mCenter.mX - mRadius, mCenter.mY};
    if (angle_point(vert[i])) {
        ++i;
    }
    vert[i] = {mCenter.mX, mCenter.mY - mRadius};
    if (angle_point(vert[i])) {
        ++i;
    }
    vert[i] = {mCenter.mX + mRadius, mCenter.mY};
    if (angle_point(vert[i])) {
        ++i;
    }
    vert[i] = {mCenter.mX, mCenter.mY + mCenter.mY};
    if (angle_point(vert[i])) {
        ++i;
    }
    return get_vertices_bouding(ArrayRef<IntPoint>(vert, i));
}

///////////////////////////////////////////////////////////////////////////
// IntABox
//////////////////////////////////////////////////////////////////////////
bool IntABox::intersect(const IntABox& box) const {
    return box.mMin.mX <= mMax.mX && box.mMin.mY <= mMax.mY && box.mMax.mX >= mMin.mX && box.mMax.mY >= mMin.mY;
}

bool IntABox::contain(const IntABox& box) const { return mMin.mX <= box.mMin.mX && mMin.mY <= box.mMin.mY && box.mMax.mX <= mMax.mX && box.mMax.mY <= mMax.mY; }

bool IntABox::contain_point(const IntPoint& p) const { return mMin.mX <= p.mX && mMax.mX >= p.mX && mMin.mY <= p.mY && mMax.mY >= p.mY; }

bool IntABox::intersect_obox(const IntOBox& obox) const {
    IntOBox self(to_obox());
    return self.inersect(obox);
}

bool IntABox::intersect_line(const IntLine& l) const {
    if (contain_point(l.mMin) || contain_point(l.mMax)) {
        return true;
    }
    IntLine l1{{mMin.mX, mMin.mY}, {mMax.mX, mMax.mY}};
    IntLine l2{{mMin.mX, mMax.mY}, {mMax.mX, mMin.mY}};
    return l.intersect(l1) || l.intersect(l2);
}

bool IntABox::intersect_circle(const IntCircle& cc) const {
    int32_t minX = cc.mCenter.mX;
    int32_t minY = cc.mCenter.mY;
    if (cc.mCenter.mX < mMin.mX) {
        minX = mMin.mX;
    } else if (cc.mCenter.mX > mMax.mX) {
        minX = mMax.mX;
    }
    if (cc.mCenter.mY < mMin.mY) {
        minY = mMin.mY;
    } else if (cc.mCenter.mY > mMax.mY) {
        minY = mMax.mY;
    }
    int32_t dX = cc.mCenter.mX - minX;
    int32_t dY = cc.mCenter.mY - minY;
    return (dX * dX + dY * dY) <= (cc.mRadius * cc.mRadius);
}

bool IntABox::intersect_sector(const IntSector& sec) const {
    IntCircle circle{sec.mCenter, sec.mRadius};
    if (!intersect_circle(circle)) {
        return false;
    }
    if (contain_point(sec.mCenter)) {
        return true;
    }
    if (sec.contain_point(mMin) || sec.contain_point(mMax)) {
        return true;
    }
    IntPoint p1{mMax.mX, mMin.mY}, p2{mMin.mX, mMax.mY};
    if (sec.contain_point(p1) || sec.contain_point(p2)) {
        return true;
    }
    IntLine line{sec.mCenter, sec.get_min_angle_point()};
    if (intersect_line(line)) {
        return true;
    }
    line.mMax = sec.get_max_angle_point();
    if (intersect_line(line)) {
        return true;
    }
    IntPoint fp[2];
    IntLine lines[4]{{mMin, {mMax.mX, mMin.mY}}, {mMin, {mMin.mX, mMax.mY}}, {{mMax.mX, mMin.mY}, mMax}, {{mMin.mX, mMax.mY}, mMax}};
    ArrayRef<IntPoint> arr(fp, 2);
    for (int i = 0; i < PURE_ARRAY_SIZE(lines); ++i) {
        int32_t c = circle.get_intersect_point(lines[i], arr);
        for (int32_t j = 0; j < c; ++j) {
            if (sec.angle_point(fp[j])) {
                return true;
            }
        }
    }
    return false;
}

int64_t IntABox::enlarge_area(const IntABox& box) const {
    return int64_t(std::max(mMax.mX, box.mMax.mX) - std::min(mMin.mX, box.mMin.mX)) * int64_t(std::max(mMax.mY, box.mMax.mY) - std::min(mMin.mY, box.mMin.mY));
}

int64_t IntABox::box_area() const { return int64_t(mMax.mX - mMin.mX) * int64_t(mMax.mY - mMin.mY); }

int64_t IntABox::box_margin() const { return int64_t(mMax.mX - mMin.mX) + int64_t(mMax.mY - mMin.mY); }

int64_t IntABox::intersect_area(const IntABox& box) const {
    auto minX = std::max(mMin.mX, box.mMin.mX);
    auto minY = std::max(mMin.mY, box.mMin.mY);
    auto maxX = std::min(mMax.mX, box.mMax.mX);
    auto maxY = std::min(mMax.mY, box.mMax.mY);
    return int64_t(std::max(0, maxX - minX)) * int64_t(std::max(0, maxY - minY));
}

void IntABox::extend(const IntABox& box) {
    mMin.mX = std::min(mMin.mX, box.mMin.mX);
    mMin.mY = std::min(mMin.mY, box.mMin.mY);
    mMax.mX = std::max(mMax.mX, box.mMax.mX);
    mMax.mY = std::max(mMax.mY, box.mMax.mY);
}

IntOBox IntABox::to_obox() const {
    IntPoint halfSize{(mMax.mX - mMin.mX) / 2, (mMax.mY - mMin.mY) / 2};
    return IntOBox{{mMin.mX + halfSize.mX, mMin.mY + halfSize.mY}, halfSize, 0.0};
}

IntPoint IntABox::center() const {
    IntPoint halfSize{(mMax.mX - mMin.mX) / 2, (mMax.mY - mMin.mY) / 2};
    return IntPoint{mMin.mX + halfSize.mX, mMin.mY + halfSize.mY};
}

IntPoint IntABox::size() const { return IntPoint{mMax.mX - mMin.mX, mMax.mY - mMin.mY}; }

IntPoint IntABox::half_size() const { return IntPoint{(mMax.mX - mMin.mX) / 2, (mMax.mY - mMin.mY) / 2}; }

///////////////////////////////////////////////////////////////////////////
// IntOBox
//////////////////////////////////////////////////////////////////////////
int IntOBox::get_vertices(ArrayRef<IntPoint>& points) const {
    if (points.size() != 4) {
        return ErrorInvalidArg;
    }
    double cosVal = std::cos(mAngle);
    double sinVal = std::sin(mAngle);
    IntPoint sideX{int32_t(mHalfSize.mX * cosVal), int32_t(mHalfSize.mX * sinVal)};
    IntPoint sideY{int32_t(-mHalfSize.mY * sinVal), int32_t(mHalfSize.mY * cosVal)};
    points[0] = {mCenter.mX - sideX.mX - sideY.mX, mCenter.mY - sideX.mY - sideY.mY};
    points[1] = {mCenter.mX + sideX.mX - sideY.mX, mCenter.mY + sideX.mY - sideY.mY};
    points[2] = {mCenter.mX + sideX.mX + sideY.mX, mCenter.mY + sideX.mY + sideY.mY};
    points[3] = {mCenter.mX - sideX.mX + sideY.mX, mCenter.mY - sideX.mY + sideY.mY};
    return Success;
}

bool IntOBox::inersect(const IntOBox& box) const {
    IntPoint pgA[4]{};
    IntPoint pgB[4]{};
    ArrayRef<IntPoint> arrA(pgA, 4);
    ArrayRef<IntPoint> arrB(pgB, 4);
    get_vertices(arrA);
    box.get_vertices(arrB);
    IntPoint axies[4]{
        {pgA[1].mX - pgA[0].mX, pgA[1].mY - pgA[0].mY},
        {pgA[3].mX - pgA[0].mX, pgA[3].mY - pgA[0].mY},
        {pgB[1].mX - pgB[0].mX, pgB[1].mY - pgB[0].mY},
        {pgB[3].mX - pgB[0].mX, pgB[3].mY - pgB[0].mY},
    };
    for (size_t i = 0; i < PURE_ARRAY_SIZE(axies); ++i) {
        if (is_separated_axis(axies[i], arrA, arrB)) {
            return false;
        }
    }
    return true;
}

IntABox IntOBox::get_bounding() const {
    IntPoint vert[4]{};
    ArrayRef<IntPoint> arr(vert, 4);
    get_vertices(arr);
    return get_vertices_bouding(arr);
}

}  // namespace PureCore