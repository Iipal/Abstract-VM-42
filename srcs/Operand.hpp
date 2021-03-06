#pragma once

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "Reader.hpp"
#include "Validation.hpp"

template<typename T>
class Operand : public IOperand {
public:
    Operand<T>();
    Operand<T>(eOperandType const &type, std::string const &valueStr, T const &value);
    Operand<T>(Operand<T> const &copy);
    virtual ~Operand<T>();

    Operand<T> &operator=(Operand<T> const &copy);

    int getPrecision(void) const;
    eOperandType getType(void) const;

    IOperand const *baseOperators(IOperand const &lOperand, char const op) const;

    IOperand const *operator+(IOperand const &lOperand) const;
    IOperand const *operator-(IOperand const &lOperand) const;
    IOperand const *operator*(IOperand const &lOperand) const;
    IOperand const *operator/(IOperand const &lOperand) const;
    IOperand const *operator%(IOperand const &lOperand) const;

    std::string const &toString(void) const;
private:
    eOperandType _type;
    std::string _valueStr;
    T _value;
};

/* definitions: */
template <typename T> Operand<T>::Operand() {}
template <typename T> Operand<T>::Operand(eOperandType const &type, std::string const &valueStr, T const &value)
    : _type(type), _valueStr(valueStr), _value(value) { }
template <typename T> Operand<T>::Operand(Operand<T> const &copy) { *this = copy; }
template <typename T> Operand<T>::~Operand() {}
template <typename T> Operand<T> &Operand<T>::operator=(Operand<T> const &copy) {
    if (this != &copy) {
        _type = copy._type;
        _valueStr = copy._valueStr;
        _value = copy._value;
    }
    return *this;
}

template <typename T> inline eOperandType Operand<T>::getType(void) const { return _type; }
template <typename T>
inline int Operand<T>::getPrecision(void) const {
    int _precision = 0;
    if (Int32 < _type) {
        const size_t _valueFloatingPointDot = _valueStr.find_first_of('.', 0);
        if (_valueFloatingPointDot < _valueStr.length()) {
            _precision = _valueStr.substr(_valueFloatingPointDot + 1, _valueStr.length() - _valueFloatingPointDot - 2).length();
        }
    }
    return _precision;
}

static bool isNewValueOverflowType(long double const newValue, eOperandType const newType) {
    bool isValid = true;
    switch (newType) {
        case Int8: {
            if (INT8_MAX < newValue || INT8_MIN > newValue) {
                std::cout << ERR << std::endl << ERR_REPORT_PREFIX << newValue
                    << " is overflow of type int8_t(" UNDERLINE << INT8_MAX << " - " << INT8_MIN << WHITE ");" << std::endl;
                isValid = false;
            } break;
        }
        case Int16: {
            if (INT16_MAX < newValue || INT16_MIN > newValue) {
                std::cout << ERR << std::endl << ERR_REPORT_PREFIX << newValue
                    << " is overflow of type int16_t(" UNDERLINE << INT16_MAX << " - " << INT16_MIN << WHITE ");" << std::endl;
                isValid = false;
            } break;
        }
        case Int32: {
            if (INT32_MAX < newValue || INT32_MIN > newValue) {
                std::cout << ERR << std::endl << ERR_REPORT_PREFIX << newValue
                    << " is overflow of type int32_t(" UNDERLINE << INT32_MAX << " - " << INT32_MIN << WHITE ");" << std::endl;
                isValid = false;
            } break;
        }
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

template <typename T>
IOperand const *Operand<T>::baseOperators(IOperand const &rOperand, char const op) const {
    bool isValid = true;
    IOperand const *out = NULL;

    eOperandType newType = (_type >= rOperand.getType()) ? _type : rOperand.getType();
    std::string const &rOperandStrValue = rOperand.toString().substr(rOperand.toString().find_first_of('(', 0) + 1, rOperand.toString().length() - 2);

    std::istringstream ss(rOperandStrValue);

    long double newValue = 0.0L;
    long double rOperandValue = 0.0L;
    ss >> rOperandValue;

    switch (op) {
        case '+': newValue = _value + rOperandValue; break;
        case '-': newValue = _value - rOperandValue; break;
        case '*': newValue = _value * rOperandValue; break;
        case '/': {
            if (!rOperandValue || !_value) {
                std::cout << ERR << std::endl << ERR_REPORT_PREFIX "one of the operands is zero, division by zero is undefined;" << std::endl;
                isValid = false;
            } else { newValue = _value / rOperandValue; }
            break;
        }
        case '%': {
            if (!rOperandValue || !_value) {
                std::cout << ERR << std::endl << ERR_REPORT_PREFIX "one of the operands is zero, malformed expression;" << std::endl;
                isValid = false;
            } else if (Int32 < newType) {
                std::cout << ERR << std::endl << ERR_REPORT_PREFIX "one of the operands is float-pointing value, malformed expression;" << std::endl;
                isValid = false;
            } else { newValue = static_cast<int32_t>(_value) % static_cast<int32_t>(rOperandValue); }
            break;
        }
    }

    isValid = isValid ? isNewValueOverflowType(newValue, newType) : isValid;

    if (isValid) {
        std::ostringstream ss;
        if (Int32 < newType) {
            ss << newValue;
        } else {
            ss << static_cast<int32_t>(newValue);
        }
        out = gOFactory.createOperand(newType, ss.str());
    }
    return out;
}

template<typename T> IOperand const *Operand<T>::operator+(IOperand const &rOperand) const { return baseOperators(rOperand, '+'); }
template<typename T> IOperand const *Operand<T>::operator-(IOperand const &rOperand) const { return baseOperators(rOperand, '-'); }
template<typename T> IOperand const *Operand<T>::operator*(IOperand const &rOperand) const { return baseOperators(rOperand, '*'); }
template<typename T> IOperand const *Operand<T>::operator/(IOperand const &rOperand) const { return baseOperators(rOperand, '/'); }
template<typename T> IOperand const *Operand<T>::operator%(IOperand const &rOperand) const { return baseOperators(rOperand, '%'); }

template<typename T> std::string const &Operand<T>::toString(void) const { return _valueStr; }
