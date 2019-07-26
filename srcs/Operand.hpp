#pragma once

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "Reader.hpp"
#include "Validation.hpp"

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

    bool isNewIntValueOverflowType(int64_t newValue, eOperandType const &newType) const {
        bool isValid = true;
        switch (newType) {
            case Int8: {
                if (INT8_MAX < newValue || INT8_MIN > newValue) {
                    std::cout << ERR << std::endl <<  ERR_REPORT_PREFIX << newValue
                        << " is overflow of type int8_t(" UNDERLINE << INT8_MAX << " - " << INT8_MIN << WHITE ");" << std::endl;
                    isValid = false;
                } break;
            }
            case Int16: {
                if (INT16_MAX < newValue || INT16_MIN > newValue) {
                    std::cout << ERR << std::endl <<  ERR_REPORT_PREFIX << newValue
                        << " is overflow of type int16_t(" UNDERLINE << INT16_MAX << " - " << INT16_MIN << WHITE ");" << std::endl;
                    isValid = false;
                } break;
            }
            case Int32: {
                if (INT32_MAX < newValue || INT32_MIN > newValue) {
                    std::cout << ERR << std::endl <<  ERR_REPORT_PREFIX << newValue
                        << " is overflow of type int32_t(" UNDERLINE << INT32_MAX << " - " << INT32_MIN << WHITE ");" << std::endl;
                    isValid = false;
                } break;
            }
            default: break;
        }
        return isValid;
    }

    bool isNewDoubleValueOverflowType(long double newValue, eOperandType const &newType) const {
        bool isValid = true;
        switch (newType) {
            case Float: {
                if (__FLT_MAX__ < newValue || __FLT_MIN__ > newValue) {
                    std::cout << ERR << std::endl <<  ERR_REPORT_PREFIX << newValue
                        << " is overflow of type float(" UNDERLINE << __FLT_MAX__ << " - " << __FLT_MIN__ << WHITE ");" << std::endl;
                    isValid = false;
                } break;
            }
            case Double: {
                if (__DBL_MAX__ < newValue || __DBL_MIN__ > newValue) {
                    std::cout << ERR << std::endl <<  ERR_REPORT_PREFIX << newValue
                        << " is overflow of type double(" UNDERLINE << __DBL_MAX__ << " - " << __DBL_MIN__ << WHITE ");" << std::endl;
                    isValid = false;
                } break;
            }
            default: break;
        }
        return isValid;
    }

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
                        std::cout << ERR << std::endl << ERR_REPORT_PREFIX
                            "one of the operands is zero, division by zero is undefined;" << std::endl;
                        isValid = false;
                    } else { newIntValue = lOperandIntValue / this->_value; }
                    break;
                }
                case '%': {
                    if (!lOperandIntValue || !this->_value) {
                        std::cout << ERR << std::endl << ERR_REPORT_PREFIX
                            "one of the operands is zero, malformed expression;" << std::endl;
                        isValid = false; break;
                    } else { newIntValue = lOperandIntValue / this->_value; }
                }
                default: break;
            }

            isValid = isValid ? isNewIntValueOverflowType(newIntValue, newType) : isValid;

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
                        std::cout << ERR << std::endl << ERR_REPORT_PREFIX
                            "one of the operands is zero, division by zero is undefined;" << std::endl;
                        isValid = false;
                    } else { newDoubleValue = lOperandDoubleValue / this->_value; }
                    break;
                }
                case '%': {
                    std::cout << ERR << std::endl << ERR_REPORT_PREFIX
                        "one of the operands is float-pointing value, malformed expression;" << std::endl;
                    isValid = false; break;
                }
                default: break;
            }

            isValid = isValid ? isNewDoubleValueOverflowType(newDoubleValue, newType) : isValid;

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
