#include "processingCommandQueue.hpp"

bool processPush(std::string const &param, std::list<IOperand const*> *const o) {
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

    o->push_front(gOFactory.createOperand(_oType, _paramValue));
    std::cout << "pushed " << _paramType << "(" << _paramValue << ") to top of the stack;" << std::endl;
    return true;
}

bool processAssert(std::string const &param, std::list<IOperand const*> *const o) {
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

    std::list<IOperand const*>::const_iterator it = o->begin();
    std::string _itParamValue = (*it)->toString();
    _itParamValue = _itParamValue.substr(_itParamValue.find_first_of('(', 0) + 1,
        _itParamValue.find_first_of(')', 0) - (_itParamValue.find_first_of('(', 0) + 1));
    if ((*it)->getType() != _oType || _itParamValue != _paramValue) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) << UNDERLINE "assert "
            << param << WHITE " is " RED "false" WHITE ";" << std::endl;
        return false;
    } else {
        std::cout << "assert " << param << " is " GREEN "true" WHITE ";" << std::endl;
    }
    return true;
}
