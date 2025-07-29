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
#include "PureCore/PureLog.h"
#include "PureCore/PureXml.h"

namespace PureCore {

///////////////////////////////////////////////////////////////////////////
// PureXmlItem
//////////////////////////////////////////////////////////////////////////
PureXmlItem::PureXmlItem(const pugi::xml_node& node) : mNode(node) {}

PureXmlItem::PureXmlItem(const PureXmlItem& cp) : mNode(cp.mNode) {}

PureXmlItem& PureXmlItem::operator=(const PureXmlItem& rhs) {
    mNode = rhs.mNode;
    return *this;
}

bool PureXmlItem::is_null() const { return mNode.empty(); }

PureXmlItem::operator bool() const { return !is_null(); }

const char* PureXmlItem::get_name() const { return mNode.name(); }

bool PureXmlItem::is_node_item() const { return mNode.type() == pugi::node_element; }

bool PureXmlItem::has_child() const { return !mNode.first_child().empty(); }

bool PureXmlItem::has_name_child(const char* name) const { return !mNode.child(name).empty(); }

PureXmlItem PureXmlItem::get_first_child() const { return mNode.first_child(); }

PureXmlItem PureXmlItem::get_name_child(const char* name) const { return mNode.child(name); }

PureXmlItem PureXmlItem::must_get_child(const char* name) {
    if (is_null()) {
        return PureXmlItem();
    }
    auto node = mNode.child(name);
    if (!node) {
        node = mNode.append_child(name);
    }
    return node;
}

PureXmlItem PureXmlItem::get_next_brother() const { return mNode.next_sibling(); }

PureXmlItem PureXmlItem::get_next_name_brother(const char* name) const { return mNode.next_sibling(name); }

PureXmlItem PureXmlItem::get_parent() const { return mNode.parent(); }

const char* PureXmlItem::get_value() const { return mNode.child_value(); }

bool PureXmlItem::has_any_attr() const { return !mNode.first_attribute().empty(); }

unsigned PureXmlItem::get_attrs_count() const {
    unsigned count = 0;
    range_attrs([&count](const char*, const char*) { ++count; });
    return count;
}

void PureXmlItem::range_attrs(std::function<void(const char* name, const char* attr)> func) const {
    pugi::xml_attribute attr = mNode.first_attribute();
    while (!attr.empty()) {
        func(attr.name(), attr.value());
        attr = attr.next_attribute();
    }
}

bool PureXmlItem::has_attr(const char* name) const { return !mNode.attribute(name).empty(); }

const char* PureXmlItem::get_attr(const char* name) const { return mNode.attribute(name).value(); }

bool PureXmlItem::get_attr_as_bool(const char* name) const { return mNode.attribute(name).as_bool(); }

int64_t PureXmlItem::get_attr_as_int(const char* name) const { return mNode.attribute(name).as_llong(); }

double PureXmlItem::get_attr_as_float(const char* name) const { return mNode.attribute(name).as_double(); }

void PureXmlItem::set_name(StringRef name) { mNode.set_name(name.data(), name.size()); }

PureXmlItem PureXmlItem::append_child(const char* name) {
    if (is_null()) {
        return PureXmlItem();
    }
    auto child = mNode.append_child(name);
    return child;
}

PureXmlItem PureXmlItem::front_child(const char* name) {
    if (is_null()) {
        return PureXmlItem();
    }
    auto child = mNode.prepend_child(name);
    return child;
}

PureXmlItem PureXmlItem::insert_after_child(const char* name, const PureXmlItem& bro) {
    if (is_null()) {
        return PureXmlItem();
    }
    auto child = mNode.insert_child_after(name, bro.mNode);
    return child;
}

PureXmlItem PureXmlItem::insert_before_child(const char* name, const PureXmlItem& bro) {
    if (is_null()) {
        return PureXmlItem();
    }
    auto child = mNode.insert_child_before(name, bro.mNode);
    return child;
}

int PureXmlItem::add_to_last_child(PureXmlItem item, bool isMove) {
    if (is_null()) {
        return ErrorNotSupport;
    }
    if (!item) {
        return ErrorInvalidArg;
    }
    if (isMove) {
        mNode.append_move(item.mNode);
    } else {
        mNode.append_copy(item.mNode);
    }
    return Success;
}

int PureXmlItem::add_to_first_child(PureXmlItem item, bool isMove) {
    if (is_null()) {
        return ErrorNotSupport;
    }
    if (!item) {
        return ErrorInvalidArg;
    }
    if (isMove) {
        mNode.prepend_move(item.mNode);
    } else {
        mNode.prepend_copy(item.mNode);
    }
    return Success;
}

int PureXmlItem::leave() { return get_parent().remove_child(*this); }

int PureXmlItem::remove_child(PureXmlItem item) {
    if (!mNode.remove_child(item.mNode)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureXmlItem::remove_name_child(const char* name) {
    if (!mNode.remove_child(name)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureXmlItem::remove_name_children(const char* name) {
    if (is_null() || name == nullptr) {
        return ErrorInvalidArg;
    }
    while (true) {
        auto child = mNode.child(name);
        if (child.empty()) {
            break;
        }
        mNode.remove_child(child);
    }
    return Success;
}

void PureXmlItem::clear_children() { mNode.remove_children(); }

int PureXmlItem::remove_attr(const char* name) {
    if (!mNode.remove_attribute(name)) {
        return ErrorNotSupport;
    }
    return Success;
}

void PureXmlItem::clear_attrs() { mNode.remove_attributes(); }

int PureXmlItem::set_value(DataRef value) {
    if (is_null() || value.empty()) {
        return ErrorInvalidArg;
    }
    for (auto child = mNode.first_child(); child; child = child.next_sibling()) {
        if (child.type() == pugi::node_pcdata) {
            if (!child.set_value(value.data(), value.size())) {
                return ErrorNotSupport;
            }
            return Success;
        }
    }
    if (!mNode.append_child(pugi::node_pcdata).set_value(value.data(), value.size())) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureXmlItem::set_attr(const char* name, const char* value) {
    if (is_null() || name == nullptr || value == nullptr) {
        return ErrorInvalidArg;
    }
    auto attr = mNode.attribute(name);
    if (attr.empty()) {
        attr = mNode.append_attribute(name);
    }
    if (!attr.set_value(value)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureXmlItem::set_attr_bool(const char* name, bool value) {
    if (is_null() || name == nullptr) {
        return ErrorInvalidArg;
    }
    auto attr = mNode.attribute(name);
    if (attr.empty()) {
        attr = mNode.append_attribute(name);
    }
    if (!attr.set_value(value)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureXmlItem::set_attr_int(const char* name, int64_t value) {
    if (is_null() || name == nullptr) {
        return ErrorInvalidArg;
    }
    auto attr = mNode.attribute(name);
    if (attr.empty()) {
        attr = mNode.append_attribute(name);
    }
    if (!attr.set_value(value)) {
        return ErrorNotSupport;
    }
    return Success;
}

int PureXmlItem::set_attr_float(const char* name, double value) {
    if (is_null() || name == nullptr) {
        return ErrorInvalidArg;
    }
    auto attr = mNode.attribute(name);
    if (attr.empty()) {
        attr = mNode.append_attribute(name);
    }
    if (!attr.set_value(value)) {
        return ErrorNotSupport;
    }
    return Success;
}

///////////////////////////////////////////////////////////////////////////
// PureXml
//////////////////////////////////////////////////////////////////////////
int PureXml::load(DataRef data) {
    auto r = mDoc.load_buffer(data.data(), data.size());
    if (!r) {
        PureError(r.description());
        return ErrorInvalidData;
    }
    return Success;
}

int PureXml::save(IBuffer& buffer, bool format) {
    class SaveXmler : public pugi::xml_writer {
    public:
        SaveXmler(IBuffer& buffer) : mBuffer(buffer) {}
        virtual ~SaveXmler() = default;
        virtual void write(const void* data, size_t size) { mErr = mBuffer.write(DataRef(data, size)); }
        int get_err() const { return mErr; }

    private:
        int mErr = 0;
        IBuffer& mBuffer;
    };
    SaveXmler sx(buffer);
    const char* indent = format ? "\t" : "";
    mDoc.save(sx, indent);
    return sx.get_err();
}

void PureXml::clear() { mDoc.reset(); }

bool PureXml::has_root() { return mDoc.first_child(); }

bool PureXml::has_name_root(const char* name) {
    if (name == nullptr) {
        return false;
    }
    return std::strcmp(mDoc.first_child().name(), name) == 0;
}

PureXmlItem PureXml::get_root() { return mDoc.first_child(); }

PureXmlItem PureXml::get_name_root(const char* name) {
    if (name == nullptr) {
        name = "";
    }
    auto root = mDoc.first_child();
    if (!root || std::strcmp(root.name(), name) != 0) {
        return PureXmlItem();
    }
    return root;
}

PureXmlItem PureXml::must_get_root(const char* name) {
    auto root = get_name_root(name);
    if (root) {
        return root;
    }
    return create_root(name);
}

PureXmlItem PureXml::create_root(const char* name) {
    if (name == nullptr) {
        name = "";
    }
    clear();
    return mDoc.append_child(name);
}

}  // namespace PureCore