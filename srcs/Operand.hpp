#pragma once

#include "IOperand.hpp"
#include "OperandFactory.hpp"
#include "Reader.hpp"

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

    IOperand const *operator+(IOperand const &rhs) const {
        IOperand const *out = NULL;
        eOperandType _newType = (this->_type > rhs.getType()) ? this->_type : rhs.getType();
        if (Float > _newType) {
            int64_t _rhsValue = std::stol(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            int32_t _newValue = _rhsValue + this->_value;
            out = gOFactory.createOperand(_newType, std::to_string(_newValue));
        } else {
            double _rhsValue = std::stod(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            double _newValue = _rhsValue + this->_value;
            out = gOFactory.createOperand(_newType, std::to_string(_newValue));
        }
        return out;
    }

    IOperand const *operator-(IOperand const &rhs) const {
        IOperand const *out = NULL;
        eOperandType _newType = (this->_type > rhs.getType()) ? this->_type : rhs.getType();
        if (Float > _newType) {
            int64_t _rhsValue = std::stol(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            int32_t _newValue = _rhsValue - this->_value;
            out = gOFactory.createOperand(_newType, std::to_string(_newValue));
        } else {
            double _rhsValue = std::stod(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            double _newValue = _rhsValue - this->_value;
            out = gOFactory.createOperand(_newType, std::to_string(_newValue));
        }
        return out;
    }

    IOperand const *operator*(IOperand const &rhs) const {
        IOperand const *out = NULL;
        eOperandType _newType = (this->_type > rhs.getType()) ? this->_type : rhs.getType();
        if (Float > _newType) {
            int64_t _rhsValue = std::stol(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            if (!_rhsValue || !this->_value) {
                std::cout << ERR_REPORT_PREFIX "one of the operands is zero. can't multiply to zero;" << std::endl;
            } else {
                int32_t _newValue = _rhsValue * this->_value;
                out = gOFactory.createOperand(_newType, std::to_string(_newValue));
            }
        } else {
            double _rhsValue = std::stod(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            if (!_rhsValue || !this->_value) {
                std::cout << ERR_REPORT_PREFIX "one of the operands is zero. can't multiply to zero;" << std::endl;
            } else {
                double _newValue = _rhsValue * this->_value;
                out = gOFactory.createOperand(_newType, std::to_string(_newValue));
            }
        }
        return out;
    }

    IOperand const *operator/(IOperand const &rhs) const {
        IOperand const *out = NULL;
        eOperandType _newType = (this->_type > rhs.getType()) ? this->_type : rhs.getType();
        if (Float > _newType) {
            int64_t _rhsValue = std::stol(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            if (!_rhsValue || !this->_value) {
                std::cout << ERR_REPORT_PREFIX "one of the operands is zero. can't divide to zero;" << std::endl;
            } else {
                int32_t _newValue = _rhsValue / this->_value;
                out = gOFactory.createOperand(_newType, std::to_string(_newValue));
            }
        } else {
            double _rhsValue = std::stod(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            if (!_rhsValue || !this->_value) {
                std::cout << ERR_REPORT_PREFIX "one of the operands is zero. can't divide to zero;" << std::endl;
            } else {
                double _newValue = _rhsValue / this->_value;
                out = gOFactory.createOperand(_newType, std::to_string(_newValue));
            }
        }
        return out;
    }

    IOperand const *operator%(IOperand const &rhs) const {IOperand const *out = NULL;
        eOperandType _newType = (this->_type > rhs.getType()) ? this->_type : rhs.getType();
        if (Float > _newType) {
            int32_t _rhsValue = std::stol(rhs.toString().substr(rhs.toString().find_first_of('(', 0) + 1, rhs.toString().length() - 2));
            if (!_rhsValue || !this->_value) {
                std::cout << ERR_REPORT_PREFIX "one of the operands is zero. can't module to zero;" << std::endl;
            } else {
                int32_t _newValue = static_cast<int32_t>(_rhsValue) % static_cast<int32_t>(this->_value);
                out = gOFactory.createOperand(_newType, std::to_string(_newValue));
            }
        } else {
            std::cout << ERR_REPORT_PREFIX "at least one of the operand is double(or float), can't process module for floating point values;" << std::endl;
        }
        return out;
    }

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
