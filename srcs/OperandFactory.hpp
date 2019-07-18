#pragma once

#include "IOperand.hpp"

class OperandFactory {
public:
    OperandFactory();
    OperandFactory(OperandFactory const &copy);
    virtual ~OperandFactory();

    IOperand const *createOperand(eOperandType type, std::string const &value) const;

    OperandFactory &operator=(OperandFactory const &copy);

    class UnknownOperandTypeOccuredException : public std::exception {
    public:
        UnknownOperandTypeOccuredException() { }
        UnknownOperandTypeOccuredException(UnknownOperandTypeOccuredException const &copy) { *this = copy; }
        virtual ~UnknownOperandTypeOccuredException() throw() { }
        UnknownOperandTypeOccuredException &operator=(UnknownOperandTypeOccuredException const &copy) { (void)copy; return *this; }

        const char *what(void) const throw() { return "Exception: Unknown operand type occured."; }
    };
private:

    typedef IOperand const* (OperandFactory::*fnptrCreateOperand)(std::string const &) const;
    static const fnptrCreateOperand fnptrCreateOperands[MaxOperandTypes];

    IOperand const *createInt8  (std::string const &value) const;
    IOperand const *createInt16 (std::string const &value) const;
    IOperand const *createInt32 (std::string const &value) const;
    IOperand const *createFloat (std::string const &value) const;
    IOperand const *createDouble(std::string const &value) const;
};

static const OperandFactory gOFactory;
