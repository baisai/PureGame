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
#include "PureCore/DataRef.h"
#include "PureCore/Buffer/IBuffer.h"

#include "pugixml.hpp"

#include <functional>

namespace PureCore {
class PURECORE_API PureXmlItem {
public:
    PureXmlItem() = default;
    PureXmlItem(const pugi::xml_node& node);
    PureXmlItem(const PureXmlItem& cp);
    PureXmlItem& operator=(const PureXmlItem& rhs);

    bool is_null() const;
    explicit operator bool() const;
    const char* get_name() const;
    bool is_node_item() const;
    bool has_child() const;
    bool has_name_child(const char* name) const;
    PureXmlItem get_first_child() const;
    PureXmlItem get_name_child(const char* name) const;
    PureXmlItem must_get_child(const char* name);
    PureXmlItem get_next_brother() const;
    PureXmlItem get_next_name_brother(const char* name) const;
    PureXmlItem get_parent() const;

    const char* get_value() const;

    bool has_any_attr() const;
    unsigned get_attrs_count() const;
    void range_attrs(std::function<void(const char* name, const char* attr)> func) const;
    bool has_attr(const char* name) const;
    const char* get_attr(const char* name) const;
    bool get_attr_as_bool(const char* name) const;
    int64_t get_attr_as_int(const char* name) const;
    double get_attr_as_float(const char* name) const;

    void set_name(StringRef name);
    PureXmlItem append_child(const char* name);
    PureXmlItem front_child(const char* name);
    PureXmlItem insert_after_child(const char* name, const PureXmlItem& bro);
    PureXmlItem insert_before_child(const char* name, const PureXmlItem& bro);
    int add_to_last_child(PureXmlItem item, bool isMove);
    int add_to_first_child(PureXmlItem item, bool isMove);
    int leave();
    int remove_child(PureXmlItem item);
    int remove_name_child(const char* name);
    int remove_name_children(const char* name);
    void clear_children();

    int remove_attr(const char* name);
    void clear_attrs();

    int set_value(DataRef value);
    int set_attr(const char* name, const char* value);
    int set_attr_bool(const char* name, bool value);
    int set_attr_int(const char* name, int64_t value);
    int set_attr_float(const char* name, double value);

private:
    pugi::xml_node mNode;
};

class PURECORE_API PureXml {
public:
    PureXml() = default;

    int load(DataRef data);
    int save(IBuffer& buffer, bool format = true);
    void clear();

    bool has_root();
    bool has_name_root(const char* name);
    PureXmlItem get_root();
    PureXmlItem get_name_root(const char* name);
    PureXmlItem must_get_root(const char* name);
    PureXmlItem create_root(const char* name);

private:
    pugi::xml_document mDoc;

    PURE_DISABLE_COPY(PureXml)
};
}  // namespace PureCore
