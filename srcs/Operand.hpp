#pragma once

#include "IOperand.hpp"

template<typename T>
class Operand : public IOperand {
public:
    Operand<T>() { }
    Operand<T>(eOperandType const &type, std::string const &valueStr, T value)
     : _type(type), _valueStr(valueStr), _value(value) { }
    Operand<T>(Operand<T> const &copy) { *this = copy; }
    virtual ~Operand<T>() { }

    Operand<T> &operator=(Operand<T> const &copy) {
        if (this != &copy) {
            this->_type = copy._type;
            this->_valueStr = copy._valueStr;
            this->_value = copy._value;
        }
        return *this;
    }

    int getPrecision(void) const {
        int _precision = 0;
        if (Double == this->_type || Float == this->_type) {
            const size_t _valueFloatingPointDot = this->_valueStr.find_first_of('.', 0);
            if (_valueFloatingPointDot < this->_valueStr.length()) {
                const std::string _valueAfterDot = this->_valueStr.substr(_valueFloatingPointDot, this->_valueStr.length() - _valueFloatingPointDot);
                _precision = _valueAfterDot.length();
            }
        }
        return _precision;
    }
    eOperandType getType(void) const { return this->_type; }

    IOperand const *operator+(IOperand const &rhs) const { (void)rhs; return new Operand; }
    IOperand const *operator-(IOperand const &rhs) const { (void)rhs; return new Operand; }
    IOperand const *operator*(IOperand const &rhs) const { (void)rhs; return new Operand; }
    IOperand const *operator/(IOperand const &rhs) const { (void)rhs; return new Operand; }
    IOperand const *operator%(IOperand const &rhs) const { (void)rhs; return new Operand; }

    std::string const toString(void) const {
        std::string _typeStr;
        {
            size_t i = ~0ULL;
            while (MaxOperandTypes > ++i) {
                if (this->_type == i) {
                    _typeStr = std::string(_validPushParamTypes[i]);
                    break ;
                }
            }
        }
        return _typeStr + "(" + this->_valueStr + ")";
    }
private:
    eOperandType _type;
    std::string _valueStr;
    T _value;
};
