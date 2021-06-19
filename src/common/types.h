#pragma once

enum class ValueType : unsigned char {
    VT_INT8 = 0,
    VT_UINT8,
    VT_INT16,
    VT_UINT16,
    VT_INT32,
    VT_UINT32,
    VT_FLOAT
};

template <ValueType Type>
struct ValueTypeSelect {
};

template <>
struct ValueTypeSelect<ValueType::VT_INT8> {
    using type = char;
};

template <>
struct ValueTypeSelect<ValueType::VT_UINT8> {
    using type = unsigned char;
};

template <>
struct ValueTypeSelect<ValueType::VT_INT16> {
    using type = short;
};

template <>
struct ValueTypeSelect<ValueType::VT_UINT16> {
    using type = unsigned short;
};

template <>
struct ValueTypeSelect<ValueType::VT_INT32> {
    using type = int;
};

template <>
struct ValueTypeSelect<ValueType::VT_UINT32> {
    using type = unsigned int;
};

template <>
struct ValueTypeSelect<ValueType::VT_FLOAT> {
    using type = float;
};
