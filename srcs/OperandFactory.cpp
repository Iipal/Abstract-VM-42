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
    std::istringstream iss(value);
    int16_t _value;
    iss >> _value;
    return new Operand<int8_t>(Int8, std::string("int8(" + value + ")"), static_cast<int8_t>(_value));
}

IOperand const *OperandFactory::createInt16(std::string const &value) const {
    std::istringstream iss(value);
    int16_t _value;
    iss >> _value;
    return new Operand<int16_t>(Int16, std::string("int16(" + value + ")"), _value);
}

IOperand const *OperandFactory::createInt32(std::string const &value) const {
    std::istringstream iss(value);
    int32_t _value;
    iss >> _value;
    return new Operand<int32_t>(Int32, std::string("int32(" + value + ")"), _value);
}

IOperand const *OperandFactory::createFloat(std::string const &value) const {
    std::istringstream iss(value);
    float _value;
    iss >> _value;
    return new Operand<float>(Float, std::string("float(" + value + ")"), _value);
}

IOperand const *OperandFactory::createDouble(std::string const &value) const {
    std::istringstream iss(value);
    double _value;
    iss >> _value;

    return new Operand<double>(Double, std::string("double(" + value + ")"), _value);
}
