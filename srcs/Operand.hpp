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

        eOperandType newType = (this->_type > lOperand.getType()) ? this->_type : lOperand.getType();
        std::string const &lOperandStrValue = lOperand.toString().substr(lOperand.toString().find_first_of('(', 0) + 1, lOperand.toString().length() - 2);

        double lOperandValue = 0.0;
        double newValue = 0.0;
        if (Float > newType) {
            lOperandValue = std::stol(lOperandStrValue);
        } else {
            lOperandValue = std::stod(lOperandStrValue);
        }
        switch (op) {
            case '+': newValue = lOperandValue + this->_value; break;
            case '-': newValue = lOperandValue - this->_value; break;
            case '*': newValue = lOperandValue * this->_value; break;
            case '/': {
                if (!lOperandValue || !this->_value) {
                    std::cout << ERR_REPORT_PREFIX "one of the operands is zero, division by zero is undefined;" << std::endl;
                    isValid = false;
                } else { newValue = lOperandValue / this->_value; }
                break;
            }
            case '%': {
                if (!lOperandValue || !this->_value) {
                    std::cout << ERR_REPORT_PREFIX "one of the operands is zero, malformed expression;" << std::endl;
                    isValid = false;
                } else if (Int32 < lOperand.getType() || Int32 > this->_type) {
                    std::cout << ERR_REPORT_PREFIX "one of the operands is float-pointing value, malformed expression;" << std::endl;
                } else { newValue = static_cast<int32_t>(lOperandValue) % static_cast<int32_t>(this->_value); }
                break;
            }
            default: break;
        }
        if (isValid) {
            out = gOFactory.createOperand(newType, std::to_string(Int32 < newType ? newValue : (static_cast<int32_t>(newValue))));
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
