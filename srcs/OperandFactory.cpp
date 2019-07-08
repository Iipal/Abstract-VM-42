#include "OperandFactory.hpp"

OperandFactory::OperandFactory() { }
OperandFactory::OperandFactory(OperandFactory const &copy) { *this = copy; }
OperandFactory::~OperandFactory() { }

OperandFactory &OperandFactory::operator=(OperandFactory const &copy) { (void)copy; return *this; }

IOperand const *OperandFactory::createOperand(eOperandType type, std::string const &value) const {
    static const fnptrCreateOperand fnptrCreateOperands[MaxOperandTypes] = {
        &OperandFactory::createInt8, &OperandFactory::createInt16,
        &OperandFactory::createInt32, &OperandFactory::createFloat,
        &OperandFactory::createDouble };

    size_t i = SIZE_T_MAX;
    while (MaxOperandTypes > ++i) {
        if (type == i) {
            return (this->*fnptrCreateOperands[type])(value);
        }
    }
    throw UnknownOperandTypeOccuredException();
    return NULL;
}

/* private methods */
IOperand const *OperandFactory::createInt8(std::string const &value) const {  }
IOperand const *OperandFactory::createInt16(std::string const &value) const {  }
IOperand const *OperandFactory::createInt32(std::string const &value) const {  }
IOperand const *OperandFactory::createFloat(std::string const &value) const {  }
IOperand const *OperandFactory::createDouble(std::string const &value) const {  }
