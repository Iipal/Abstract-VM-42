#include "Reader.hpp"
#include "OperandFactory.hpp"
#include <list>

static const OperandFactory gOFactory;

static bool processPush(std::string const &param, std::list<IOperand const*> *const operands) {
    const size_t _startBracket = param.find_first_of('(', 0) + 1;
    const size_t _endBracket = param.find_first_of(')', 0);
    const std::string _paramValue = param.substr(_startBracket, _endBracket - _startBracket);

    eOperandType _oType = MaxOperandTypes;
    {
        const std::string _paramType = param.substr(0, _startBracket - 1);
        size_t i = ~0ULL;
        while (MaxOperandTypes > ++i) {
            if (_validPushParamTypes[i] == _paramType) {
                _oType = static_cast<eOperandType>(i);
            }
        }
    }
    operands->push_front(gOFactory.createOperand(_oType, _paramValue));
}

void processingCommandQueue(std::vector<std::string> *commandQueue) {
    OperandFactory of;
    std::vector<std::string>::const_iterator it = commandQueue->begin();
    std::list<IOperand const*> operands;
    while (commandQueue->end() != it) {
        if (!(*it).compare(0, 4, "push")) {
            processPush((*it).substr(5, (*it).length() - 4), &operands);
        }
        ++it;
    }
}
