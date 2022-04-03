//
// Created by Jerry Chou on 2022/4/3.
//

#include "ConstantPoolItem.hpp"

#include <utility>

ConstantPoolItem::ItemValue::ItemValue() : BytesValue(nullptr) {

}

ConstantPoolItem::ItemValue::ItemValue(XBytes *BytesValue) : BytesValue(BytesValue) {

}

ConstantPoolItem::ItemValue::ItemValue(XString *StringValue) : StringValue(StringValue) {

}

XBytes &ConstantPoolItem::ItemValue::GetBytesValue() const {
    return *BytesValue;
}

XString &ConstantPoolItem::ItemValue::GetStringValue() const {
    return *StringValue;
}

ConstantPoolItem::ConstantPoolItem() : Kind(ItemKind::NotAllocated), Value() {

}

ConstantPoolItem::ConstantPoolItem(ConstantPoolItem::ItemKind Kind) : Kind(Kind) {
    switch (Kind) {
        case ItemKind::Bytes: {
            Value = (ItemValue) {new XBytes()};
            break;
        }
        case ItemKind::String: {
            Value = (ItemValue) {new XString()};
            break;
        }
        default: {
            Kind = ItemKind::NotAllocated;
            Value = {};
        }
    }
}

ConstantPoolItem::~ConstantPoolItem() {
    switch (Kind) {
        case ItemKind::Bytes: {
            delete Value.BytesValue;
            break;
        }
        case ItemKind::String: {
            delete Value.StringValue;
            break;
        }
        default: {
            break;
        }
    }
    Kind = ItemKind::NotAllocated;
    Value = {};
}

ConstantPoolItem::ConstantPoolItem(const ConstantPoolItem &rhs) {
    Kind = rhs.Kind;
    switch (Kind) {
        case ItemKind::Bytes: {
            Value = (ItemValue) {new XBytes()};
            Value.GetBytesValue() = rhs.Value.GetBytesValue();
            break;
        }
        case ItemKind::String: {
            Value = (ItemValue) {new XString()};
            Value.GetStringValue() = rhs.Value.GetStringValue();
            break;
        }
        default: {
            Value = {};
            break;
        }
    }
}

bool ConstantPoolItem::operator==(const ConstantPoolItem &rhs) const {
    if (Kind != rhs.Kind) return false;
    switch (Kind) {
        case ItemKind::Bytes: {
            return Value.GetBytesValue() == rhs.Value.GetBytesValue();
        }
        case ItemKind::String: {
            return Value.GetStringValue() == rhs.Value.GetStringValue();
        }
        default: {
            return true;
            break;
        }
    }
}

ConstantPoolItem::ConstantPoolItem(XString StringValue) : Kind(ItemKind::String) {
    Value = (ItemValue) {new XString()};
    Value.GetStringValue() = std::move(StringValue);
}

ConstantPoolItem::ConstantPoolItem(XBytes BytesValues) : Kind(ItemKind::Bytes) {
    Value = (ItemValue) {new XBytes()};
    Value.GetBytesValue() = std::move(BytesValues);
}
