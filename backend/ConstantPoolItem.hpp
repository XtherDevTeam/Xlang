//
// Created by Jerry Chou on 2022/4/3.
//

#ifndef XLANG2_CONSTANTPOOLITEM_HPP
#define XLANG2_CONSTANTPOOLITEM_HPP

#include "../share/config.hpp"

class ConstantPoolItem {
public:
    enum class ItemKind : int {
        NotAllocated,
        Bytes,
        String,
    } Kind;

    union ItemValue {
        XBytes *BytesValue;
        XString *StringValue;

        ItemValue();

        explicit ItemValue(XBytes *BytesValue);

        explicit ItemValue(XString *StringValue);

        [[nodiscard]] XBytes &GetBytesValue() const;

        [[nodiscard]] XString &GetStringValue() const;
    } Value;

    bool operator==(const ConstantPoolItem &rhs) const;

    ConstantPoolItem();

    ConstantPoolItem(const ConstantPoolItem &rhs);

    explicit ConstantPoolItem(ItemKind Kind);

    explicit ConstantPoolItem(XString StringValue);

    explicit ConstantPoolItem(XBytes BytesValues);

    ~ConstantPoolItem();
};


#endif //XLANG2_CONSTANTPOOLITEM_HPP
