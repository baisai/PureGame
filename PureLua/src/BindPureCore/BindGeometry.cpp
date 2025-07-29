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

#include "PureLua/LuaRegisterClass.h"

namespace PureLua {
void bind_core_geometry(lua_State* L) {
    using namespace PureCore;
    PureLua::LuaModule lm(L, "PureCore");
    lm[PureLua::LuaRegisterClass<IntPoint>(L, "IntPoint")
           .def_ctor(
               [](int32_t x, int32_t y) {
                   return new IntPoint{x, y};
               },
               [](IntPoint* p) { delete p; })
           .def(&IntPoint::sub, "sub")
           .def(&IntPoint::add, "add")
           .def(&IntPoint::mul, "mul")
           .def(&IntPoint::div, "div")
           .def(&IntPoint::dist, "dist")
           .def(&IntPoint::dist2, "dist2")
           .def(&IntPoint::dot, "dot")
           .def(&IntPoint::cross, "cross")
           .def(&IntPoint::projection, "projection")
           .def(&IntPoint::get_bounding, "get_bounding")
           .def(&IntPoint::mX, "x")
           .def(&IntPoint::mY, "y") +
       PureLua::LuaRegisterClass<IntLine>(L, "IntLine")
           .def_ctor(
               [](int32_t minX, int32_t minY, int32_t maxX, int32_t maxY) {
                   return new IntLine{minX, minY, maxX, maxY};
               },
               [](IntLine* l) { delete l; })
           .def(&IntLine::get_intersect_point, "get_intersect_point")
           .def(&IntLine::intersect, "intersect")
           .def(&IntLine::get_bounding, "get_bounding")
           .def(&IntLine::mMin, "min")
           .def(&IntLine::mMax, "max")
           .def([](IntLine& self, int32_t x) { self.mMin.mX = x; }, "set_min_x")
           .def([](const IntLine& self) { return self.mMin.mX; }, "get_min_x")
           .def([](IntLine& self, int32_t y) { self.mMin.mY = y; }, "set_min_y")
           .def([](const IntLine& self) { return self.mMin.mY; }, "get_min_y")
           .def([](IntLine& self, int32_t x) { self.mMax.mX = x; }, "set_max_x")
           .def([](const IntLine& self) { return self.mMax.mX; }, "get_max_x")
           .def([](IntLine& self, int32_t y) { self.mMax.mY = y; }, "set_max_y")
           .def([](const IntLine& self) { return self.mMax.mY; }, "get_max_y") +
       PureLua::LuaRegisterClass<IntCircle>(L, "IntCircle")
           .def_ctor(
               [](int32_t cx, int32_t cy, int32_t radius) {
                   return new IntCircle{cx, cy, radius};
               },
               [](IntCircle* c) { delete c; })
           .def(
               [](lua_State* L) {
                   IntCircle& self = PureLua::LuaStack<IntCircle&>::get(L, 1);
                   const IntLine& line = PureLua::LuaStack<const IntLine&>::get(L, 2);
                   IntPoint ps[2]{};
                   ArrayRef<IntPoint> arr(ps, 2);
                   int c = self.get_intersect_point(line, arr);
                   for (int i = 0; i < c; ++i) {
                       PureLua::LuaStack<IntPoint>::push(L, ps[i]);
                   }
                   return c;
               },
               "get_intersect_point")
           .def(&IntCircle::get_bounding, "get_bounding")
           .def(&IntCircle::mCenter, "center")
           .def(&IntCircle::mRadius, "radius")
           .def([](IntCircle& self, int32_t cx) { self.mCenter.mX = cx; }, "set_center_x")
           .def([](const IntCircle& self) { return self.mCenter.mX; }, "get_center_x")
           .def([](IntCircle& self, int32_t cy) { self.mCenter.mY = cy; }, "set_center_y")
           .def([](const IntCircle& self) { return self.mCenter.mY; }, "get_center_y") +
       PureLua::LuaRegisterClass<IntSector>(L, "IntSector")
           .def_ctor(
               [](int32_t cx, int32_t cy, int32_t radius, double minAngle, double maxAngle) {
                   return new IntSector{cx, cy, radius, minAngle, maxAngle};
               },
               [](IntSector* sec) { delete sec; })
           .def(&IntSector::angle_point, "angle_point")
           .def(&IntSector::contain_point, "contain_point")
           .def(&IntSector::get_min_angle_point, "get_min_angle_point")
           .def(&IntSector::get_max_angle_point, "get_max_angle_point")
           .def(&IntSector::get_bounding, "get_bounding")
           .def([](IntSector& self, int32_t cx) { self.mCenter.mX = cx; }, "set_center_x")
           .def([](const IntSector& self) { return self.mCenter.mX; }, "get_center_x")
           .def([](IntSector& self, int32_t cy) { self.mCenter.mY = cy; }, "set_center_y")
           .def([](const IntSector& self) { return self.mCenter.mY; }, "get_center_y") +
       PureLua::LuaRegisterClass<IntABox>(L, "IntABox")
           .def_ctor(
               [](int32_t minX, int32_t minY, int32_t maxX, int32_t maxY) {
                   return new IntABox{minX, minY, maxX, maxY};
               },
               [](IntABox* box) { delete box; })
           .def(&IntABox::intersect, "intersect")
           .def(&IntABox::contain, "contain")
           .def(&IntABox::contain_point, "contain_point")
           .def(&IntABox::intersect_obox, "intersect_obox")
           .def(&IntABox::intersect_line, "intersect_line")
           .def(&IntABox::intersect_circle, "intersect_circle")
           .def(&IntABox::intersect_sector, "intersect_sector")
           .def(&IntABox::enlarge_area, "enlarge_area")
           .def(&IntABox::box_area, "box_area")
           .def(&IntABox::box_margin, "box_margin")
           .def(&IntABox::intersect_area, "intersect_area")
           .def(&IntABox::extend, "extend")
           .def(&IntABox::to_obox, "to_obox")
           .def(&IntABox::center, "center")
           .def(&IntABox::size, "size")
           .def(&IntABox::half_size, "half_size")
           .def(&IntABox::mMin, "min")
           .def(&IntABox::mMax, "max")
           .def([](IntABox& self, int32_t x) { self.mMin.mX = x; }, "set_min_x")
           .def([](const IntABox& self) { return self.mMin.mX; }, "get_min_x")
           .def([](IntABox& self, int32_t y) { self.mMin.mY = y; }, "set_min_y")
           .def([](const IntABox& self) { return self.mMin.mY; }, "get_min_y")
           .def([](IntABox& self, int32_t x) { self.mMax.mX = x; }, "set_max_x")
           .def([](const IntABox& self) { return self.mMax.mX; }, "get_max_x")
           .def([](IntABox& self, int32_t y) { self.mMax.mY = y; }, "set_max_y")
           .def([](const IntABox& self) { return self.mMax.mY; }, "get_max_y") +
       PureLua::LuaRegisterClass<IntOBox>(L, "IntOBox")
           .def_ctor(
               [](int32_t cx, int32_t cy, int32_t hw, int32_t hh, double angle) {
                   return new IntOBox{cx, cy, hw, hh, angle};
               },
               [](IntOBox* box) { delete box; })
           .def(
               [](lua_State* L) -> int {
                   IntOBox& self = PureLua::LuaStack<IntOBox&>::get(L, 1);
                   IntPoint ps[4]{};
                   ArrayRef<IntPoint> arr(ps, 4);
                   int err = self.get_vertices(arr);
                   if (err != PureCore::Success) {
                       PureLuaErrorJump(L, "get_vertices failed `{}`", PureCore::get_error_desc(err));
                       return 0;
                   }
                   for (int i = 0; i < PURE_ARRAY_SIZE(ps); ++i) {
                       PureLua::LuaStack<IntPoint>::push(L, ps[i]);
                   }
                   return int(PURE_ARRAY_SIZE(ps));
               },
               "get_vertices")
           .def(&IntOBox::inersect, "inersect")
           .def(&IntOBox::get_bounding, "get_bounding")
           .def(&IntOBox::mCenter, "center")
           .def(&IntOBox::mHalfSize, "half_size")
           .def(&IntOBox::mAngle, "angle")
           .def([](IntOBox& self, int32_t cx) { self.mCenter.mX = cx; }, "set_center_x")
           .def([](const IntOBox& self) { return self.mCenter.mX; }, "get_center_x")
           .def([](IntOBox& self, int32_t y) { self.mCenter.mY = y; }, "set_center_y")
           .def([](const IntOBox& self) { return self.mCenter.mY; }, "get_center_y")
           .def([](IntOBox& self, int32_t hw) { self.mHalfSize.mX = hw; }, "set_half_width")
           .def([](const IntOBox& self) { return self.mHalfSize.mX; }, "get_half_width")
           .def([](IntOBox& self, int32_t hh) { self.mHalfSize.mY = hh; }, "set_half_height")
           .def([](const IntOBox& self) { return self.mHalfSize.mY; }, "get_half_height")

    ];
}

}  // namespace PureLua
