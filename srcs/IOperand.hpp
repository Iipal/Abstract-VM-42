#pragma once

#include <iostream>

#define MAX_VALID_NO_PARAM_COMMANDS 9
#define MAX_VALID_W_PARAM_COMMANDS 2

enum eOperandType { Int8, Int16, Int32, Float, Double, MaxOperandTypes };

static const std::string _validPushParamTypes[MaxOperandTypes] = { "int8", "int16", "int32",
                                                                        "float", "double" };
static const std::string _validCommandsNoParams[MAX_VALID_NO_PARAM_COMMANDS] = { "print", "exit",
                                                                "add", "sub", "mul", "div", "mod",
                                                                                    "pop", "dump" };
static const std::string _validCommandsWithParams[MAX_VALID_W_PARAM_COMMANDS] = { "push", "assert" };


class IOperand {
public:
    virtual int          getPrecision(void) const = 0;
    virtual eOperandType getType     (void) const = 0;

    virtual IOperand const *operator+(IOperand const &rhs) const = 0;
    virtual IOperand const *operator-(IOperand const &rhs) const = 0;
    virtual IOperand const *operator*(IOperand const &rhs) const = 0;
    virtual IOperand const *operator/(IOperand const &rhs) const = 0;
    virtual IOperand const *operator%(IOperand const &rhs) const = 0;

    virtual std::string const toString(void) const = 0;

    virtual ~IOperand(void) { }
};
