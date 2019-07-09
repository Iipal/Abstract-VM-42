#pragma once

#include "IOperand.hpp"

template<typename T>
class Operand : public IOperand {
public:
    Operand<T>();
    Operand<T>(Operand<T> const &copy);
    virtual ~Operand<T>();

    Operand<T> &operator=(Operand<T> const &copy);

    int          getPrecision(void) const;
    eOperandType getType     (void) const;

    IOperand const *operator+(IOperand const &rhs) const;
    IOperand const *operator-(IOperand const &rhs) const;
    IOperand const *operator*(IOperand const &rhs) const;
    IOperand const *operator/(IOperand const &rhs) const;
    IOperand const *operator%(IOperand const &rhs) const;

    std::string const &toString(void) const;
protected:
    std::string _value;
};
