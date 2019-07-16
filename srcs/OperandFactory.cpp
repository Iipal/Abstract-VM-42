#include "OperandFactory.hpp"

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

    try {
        throw UnknownOperandTypeOccuredException();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return NULL;
}

/* private methods */
IOperand const *OperandFactory::createInt8(std::string const &value) const {
    std::string _tempValue = std::string(value);
    return new Operand<int8_t>(Int8, value, std::stoi(_tempValue));
}
IOperand const *OperandFactory::createInt16(std::string const &value) const {
    return new Operand<int16_t>(Int16, value, std::stoi(value));
}
IOperand const *OperandFactory::createInt32(std::string const &value) const {
    return new Operand<int32_t>(Int32, value, std::stoi(value));
}
IOperand const *OperandFactory::createFloat(std::string const &value) const {
    return new Operand<float>(Float, value, std::stof(value));
}
IOperand const *OperandFactory::createDouble(std::string const &value) const {
    return new Operand<double>(Double, value, std::stod(value));
}
