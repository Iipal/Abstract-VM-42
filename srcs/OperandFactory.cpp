#include "OperandFactory.hpp"
#include "Operand.hpp"

const OperandFactory::fnptrCreateOperand
 OperandFactory::fnptrCreateOperands[MaxOperandTypes] = {
        &OperandFactory::createInt8, &OperandFactory::createInt16,
        &OperandFactory::createInt32, &OperandFactory::createFloat,
        &OperandFactory::createDouble };

OperandFactory::OperandFactory() { }
OperandFactory::OperandFactory(OperandFactory const &copy) { *this = copy; }
OperandFactory::~OperandFactory() { }

OperandFactory &OperandFactory::operator=(OperandFactory const &copy) { (void)copy; return *this; }

IOperand const *OperandFactory::createOperand(eOperandType type, std::string const &value) const {
    size_t i = ~0ULL;
    while (MaxOperandTypes > ++i) {
        if (type == static_cast<eOperandType>(i)) {
            return (this->*fnptrCreateOperands[type])(value);
        }
    }
    return NULL;
}

/* private methods */
IOperand const *OperandFactory::createInt8(std::string const &value) const {
    std::string _strValue = std::string("int8(" + value + ")");
    return new Operand<int8_t>(Int8, _strValue, static_cast<int8_t>(std::stoi(value)));
}
IOperand const *OperandFactory::createInt16(std::string const &value) const {
    std::string _strValue = std::string("int16(" + value + ")");
    return new Operand<int16_t>(Int16, _strValue, static_cast<int16_t>(std::stoi(value)));
}
IOperand const *OperandFactory::createInt32(std::string const &value) const {
    std::string _strValue = std::string("int32(" + value + ")");
    return new Operand<int32_t>(Int32, _strValue, std::stoi(value));
}
IOperand const *OperandFactory::createFloat(std::string const &value) const {
    std::string _value = value;
    const size_t _valueFloatingPointDot = _value.find_first_of('.', 0);
    if (_valueFloatingPointDot < _value.length()) {
        std::string _valueAfterDot = _value.substr(_valueFloatingPointDot + 1, _value.length() - _valueFloatingPointDot);
        _valueAfterDot.erase(std::find_if(_valueAfterDot.rbegin(), _valueAfterDot.rend(), [](int ch){ return ch != '0'; }).base(), _valueAfterDot.end());
        _value.erase(_valueFloatingPointDot + 1);
        _value.append(_valueAfterDot);
    }
    std::string _strValue = std::string("float(" + value + ")");
    return new Operand<float>(Float, _strValue, std::stof(value));
}
IOperand const *OperandFactory::createDouble(std::string const &value) const {
    std::string _value = value;
    const size_t _valueFloatingPointDot = _value.find_first_of('.', 0);
    if (_valueFloatingPointDot < _value.length()) {
        std::string _valueAfterDot = _value.substr(_valueFloatingPointDot + 1, _value.length() - _valueFloatingPointDot);
        _valueAfterDot.erase(std::find_if(_valueAfterDot.rbegin(), _valueAfterDot.rend(), [](int ch){ return ch != '0'; }).base(), _valueAfterDot.end());
        _value.erase(_valueFloatingPointDot + 1);
        _value.append(_valueAfterDot);
    }
    std::string _strValue = std::string("double(" + value + ")");
    return new Operand<double>(Double, _strValue, std::stod(value));
}
