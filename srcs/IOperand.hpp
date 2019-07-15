#pragma once

#include <iostream>

enum eOperandType { Int8, Int16, Int32, Float, Double, MaxOperandTypes };
static const std::string _validPushParamTypes[MaxOperandTypes] = { "int8", "int16", "int32",
                                                                        "float", "double" };


class IOperand {
public:
    virtual int          getPrecision(void) const = 0;
    virtual eOperandType getType     (void) const = 0;

    virtual IOperand const *operator+(IOperand const &rhs) const = 0;
    virtual IOperand const *operator-(IOperand const &rhs) const = 0;
    virtual IOperand const *operator*(IOperand const &rhs) const = 0;
    virtual IOperand const *operator/(IOperand const &rhs) const = 0;
    virtual IOperand const *operator%(IOperand const &rhs) const = 0;

    virtual std::string const &toString(void) const = 0;

    virtual ~IOperand(void) { }
};
