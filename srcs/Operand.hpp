#pragma once

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "Reader.hpp"

template<typename T>
class Operand : public IOperand {
public:
    Operand<T>() { }
    Operand<T>(eOperandType const &type, std::string const &valueStr, T const &value)
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
                std::string _valueAfterDot = this->_valueStr.substr(_valueFloatingPointDot + 1, this->_valueStr.length() - _valueFloatingPointDot - 2);
                _valueAfterDot.erase(std::find_if(_valueAfterDot.rbegin(), _valueAfterDot.rend(), [](int ch){ return ch != '0'; }).base(), _valueAfterDot.end());
                _precision = _valueAfterDot.length();
            }
        }
        return _precision;
    }

    eOperandType getType(void) const { return this->_type; }

    IOperand const *baseOperators(IOperand const &lOperand, char const op) const {
        bool isValid = true;
        IOperand const *out = NULL;

        eOperandType newType = (this->_type >= lOperand.getType()) ? this->_type : lOperand.getType();
        std::string const &lOperandStrValue = lOperand.toString().substr(lOperand.toString().find_first_of('(', 0) + 1, lOperand.toString().length() - 2);

        if (Float > newType) {
            int64_t lOperandIntValue = std::stol(lOperandStrValue), newIntValue = 0;
            switch (op) {
                case '+': newIntValue = lOperandIntValue + this->_value; break;
                case '-': newIntValue = lOperandIntValue - this->_value; break;
                case '*': newIntValue = lOperandIntValue * this->_value; break;
                case '/': {
                    if (!lOperandIntValue || !this->_value) {
                        std::cout << ERR_REPORT_PREFIX "one of the operands is zero, division by zero is undefined;" << std::endl;
                        isValid = false;
                    } else { newIntValue = lOperandIntValue / this->_value; }
                    break;
                }
                case '%': {
                    if (!lOperandIntValue || !this->_value) {
                        std::cout << ERR_REPORT_PREFIX "one of the operands is zero, malformed expression;" << std::endl;
                    } else { newIntValue = lOperandIntValue / this->_value; }
                    isValid = false; break;
                }
                default: break;
            }
            if (isValid) {
                out = gOFactory.createOperand(newType, std::to_string(newIntValue));
            }
        } else {
            long double lOperandDoubleValue = std::stold(lOperandStrValue), newDoubleValue = 0.0L;
            switch (op) {
                case '+': newDoubleValue = lOperandDoubleValue + this->_value; break;
                case '-': newDoubleValue = lOperandDoubleValue - this->_value; break;
                case '*': newDoubleValue = lOperandDoubleValue * this->_value; break;
                case '/': {
                    if (!lOperandDoubleValue || !this->_value) {
                        std::cout << ERR_REPORT_PREFIX "one of the operands is zero, division by zero is undefined;" << std::endl;
                        isValid = false;
                    } else { newDoubleValue = lOperandDoubleValue / this->_value; }
                    break;
                }
                case '%': {
                    std::cout << ERR_REPORT_PREFIX "one of the operands is float-pointing value, malformed expression;" << std::endl;
                    isValid = false; break;
                }
                default: break;
            }
            if (isValid) {
                out = gOFactory.createOperand(newType, std::to_string(newDoubleValue));
            }
        }
        return out;
    }

    IOperand const *operator+(IOperand const &lOperand) const { return baseOperators(lOperand, '+'); }
    IOperand const *operator-(IOperand const &lOperand) const { return baseOperators(lOperand, '-'); }
    IOperand const *operator*(IOperand const &lOperand) const { return baseOperators(lOperand, '*'); }
    IOperand const *operator/(IOperand const &lOperand) const { return baseOperators(lOperand, '/'); }
    IOperand const *operator%(IOperand const &lOperand) const { return baseOperators(lOperand, '%'); }

    std::string const &toString(void) const { return this->_valueStr; }
private:
    eOperandType _type;
    std::string _valueStr;
    T _value;
};
