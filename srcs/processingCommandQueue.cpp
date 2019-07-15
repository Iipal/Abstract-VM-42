#include "Reader.hpp"
#include "OperandFactory.hpp"
#include <list>

static const OperandFactory gOFactory;
typedef bool (*fnptrProcessCommandWParam)(std::string const&, std::list<IOperand const*> *const);
bool processPush(std::string const &param, std::list<IOperand const*> *const operands);
bool processAssert(std::string const &param, std::list<IOperand const*> *const operands);

bool processingCommandQueue(std::vector<std::string> *commandQueue) {
    const fnptrProcessCommandWParam fnptrsCommandsWParam[MAX_VALID_W_PARAM_COMMANDS] = { processPush, processAssert };

    std::list<IOperand const*> operands;

    bool isValid = true;
    std::vector<std::string>::const_iterator it = commandQueue->begin();
    while (commandQueue->end() != it) {
        for (size_t i = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++i;) {
            if (!(*it).compare(0, _validCommandsWithParams[i].length(), _validCommandsWithParams[i])) {
                if (!fnptrsCommandsWParam[i]((*it).substr(_validCommandsWithParams[i].length() + 1, (*it).length() - 2), &operands)) {
                    isValid = false;
                }
            }
        }
        ++it;
    }
    if (isValid) {
        std::cout << "  " GREEN "SUCCESSFUL" WHITE " executed AVM;" << std::endl;
    }
    return isValid;
}

bool processPush(std::string const &param, std::list<IOperand const*> *const operands) {
    const size_t _startBracket = param.find_first_of('(', 0) + 1;
    const size_t _endBracket = param.find_first_of(')', 0);
    const std::string _paramValue = param.substr(_startBracket, _endBracket - _startBracket);

    eOperandType _oType = MaxOperandTypes;
    const std::string _paramType = param.substr(0, _startBracket - 1);

    size_t i = ~0ULL;
    while (MaxOperandTypes > ++i) {
        if (_validPushParamTypes[i] == _paramType) {
            _oType = static_cast<eOperandType>(i);
        }
    }

    operands->push_front(gOFactory.createOperand(_oType, _paramValue));
    return true;
}

bool processAssert(std::string const &param, std::list<IOperand const*> *const operands) {
    const size_t _startBracket = param.find_first_of('(', 0) + 1;
    const size_t _endBracket = param.find_first_of(')', 0);
    const std::string _paramValue = param.substr(_startBracket, _endBracket - _startBracket);

    eOperandType _oType = MaxOperandTypes;
    const std::string _paramType = param.substr(0, _startBracket - 1);

    size_t i = ~0ULL;
    while (MaxOperandTypes > ++i) {
        if (_validPushParamTypes[i] == _paramType) {
            _oType = static_cast<eOperandType>(i);
        }
    }

    std::list<IOperand const*>::const_iterator it = operands->begin();
    if ((*it)->getType() != _oType || (*it)->toString() != _paramValue) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) << UNDERLINE "assert " << param
        << WHITE " is " RED "false" WHITE ". STOP executing." << std::endl;
        return false;
    }
    return true;
}
