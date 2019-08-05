#include "LaunchAVM.hpp"
#include "Validation.hpp"

LaunchAVM::LaunchAVM() { }
LaunchAVM::LaunchAVM(const LaunchAVM &copy) { *this = copy; }
LaunchAVM::~LaunchAVM() { if (!_isClear) { clear(); } }

LaunchAVM &LaunchAVM::operator=(const LaunchAVM &copy) {
    if (this != &copy) {
        this->_operands = NULL;
        this->_isClear = true;
    }
    return *this;
}

bool LaunchAVM::launchAVM(std::vector<std::string> *commandQueue) {
    Validation::refreshGlobalErrorsCounter();

    _operands = new std::list<IOperand const*>();
    if (!_operands) {
        std::cout << ERR_N_PREFIX "can't allocate memory;" << std::endl;
        _isClear = true;
        return _operands;
    } else { _isClear = false; }

    std::cout << std::endl << AVM_PREFIX UNDERLINE "start" WHITE " executing:" << std::endl << std::endl;

    bool isValid = true;
    bool _exit = false;
    size_t commandsCounter = 0;
    std::vector<std::string>::iterator it = commandQueue->begin();
    while (!_exit && commandQueue->end() != it) {
        ++commandsCounter;
        for (size_t i = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++i;) {
            if (!(*it).compare(0, _validCommandsWithParams[i].length(), _validCommandsWithParams[i])) {
                if (!(this->*fnptrsCommandsWParam[i])((*it).substr(_validCommandsWithParams[i].length() + 1, (*it).length() - 2))) {
                    isValid = false;
                }
                break ;
            }
        }

        for (size_t i = ~0ULL; MAX_VALID_NO_PARAM_COMMANDS > ++i;) {
            if (*it == _validCommandsNoParams[i]) {
                if (!i) {
                    _exit = true;
                } else {
                    if (!(this->*fnptrsCommandsNoParam[i])()) {
                        isValid = false;
                    }
                }
                break ;
            }
        }
        ++it;
    }

    std::cout << std::endl;
    if (isValid && _exit) {
        if (commandsCounter != commandQueue->size()) {
            displayUnexecutedCommands(commandQueue, it, commandsCounter);
        } else {
            std::cout << AVM_PREFIX UNDERLINE "successful" WHITE " executed;" << std::endl;
        }
    } else if (isValid && !_exit) {
        std::cout << WARN_PREFIX "executing was stopped without \'" CYAN "exit" WHITE "\';" << std::endl;
    } else {
        std::cout << WARN_PREFIX "at least [" RED UNDERLINE
            << std::setw(6) << Validation::getGlobalErrorsCounter()
            << WHITE "] error occured while AVM was executed, try to fix all error reports above for successful AVM work;" << std::endl;
    }
    clear();
    return isValid;
}

/* private methods */

void LaunchAVM::clear() {
    if (_operands) {
        std::list<IOperand const*>::iterator it = _operands->begin();
        while (_operands->end() != it) {
            delete *it;
            ++it;
        }
        delete _operands; _operands = NULL;
        _isClear = true;
    }
}

bool LaunchAVM::parsePush(std::string const &param) {
    const size_t _startBracket = param.find_first_of('(', 0) + 1;
    const size_t _endBracket = param.find_first_of(')', 0);
    const std::string _paramValue = param.substr(_startBracket, _endBracket - _startBracket);

    eOperandType _oType = MaxOperandTypes;
    const std::string _paramType = param.substr(0, _startBracket - 1);

    size_t i = ~0ULL;
    while (MaxOperandTypes > ++i) {
        if (_validPushParamTypes[i] == _paramType) {
            _oType = static_cast<eOperandType>(i); break;
        }
    }

    _operands->push_front(gOFactory.createOperand(_oType, _paramValue));
    std::cout << " " BLUE "push" WHITE ": " << _paramType << '(' << _paramValue << ");" << std::endl;
    return true;
}

bool LaunchAVM::parseAssert(std::string const &param) {
    if (!_operands->size()) {
        std::cout << ERR_N_PREFIX "any values currently pushed, can't parse \'assert\';" << std::endl;
        return false;
    }

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

    std::list<IOperand const*>::const_iterator it = _operands->begin();
    std::string itParamValue = (*it)->toString();
    itParamValue = itParamValue.substr(itParamValue.find_first_of('(', 0) + 1,
        itParamValue.find_first_of(')', 0) - (itParamValue.find_first_of('(', 0) + 1));
    std::cout << " " MAGENTA "assert" WHITE ": ";
    if ((*it)->getType() != _oType || itParamValue != _paramValue) {
        std::cout << ERR_N_PREFIX
            << param << " is " RED "false" WHITE ";" << std::endl;
        return false;
    } else {
        std::cout << param << " is " GREEN "true" WHITE ";" << std::endl;
    }
    return true;
}

bool LaunchAVM::parseExit() { return true; }

bool LaunchAVM::parsePop() {
    if (!_operands->size()) {
        std::cout << ERR_N_PREFIX "any values currently pushed, \'pop\' can't unstack value from top;" << std::endl;
        return false;
    } else {
        std::cout << " " RED "pop" WHITE ": " << (*(_operands->begin()))->toString() << ';' << std::endl;
        delete *(_operands->begin());
        _operands->pop_front();
    }
    return true;
}

bool LaunchAVM::parsePrint() {
    bool isValid = true;
    std::cout << " " CYAN "print" WHITE ": ";
    if (!_operands->size()) {
        std::cout << ERR << std::endl << ERR_N_PREFIX
            "any values currently pushed, \'print\' can't display top value;" << std::endl;
        isValid = false;
    } else {
        IOperand const *printOp = *(_operands->begin());
        if (Int8 == printOp->getType()) {
            std::string const printValueStr = printOp->toString();
            std::istringstream iss(printValueStr.substr(printValueStr.find_first_of('(', 0) + 1, printValueStr.length() - printValueStr.find_first_of('(', 0) - 2));
            int32_t printValue = 0;
            iss >> printValue;
            if (0x20 <= printValue && 0x7f > printValue) {
                std::cout << '\'' << static_cast<char>(printValue) << "\';" << std::endl;
            } else {
                std::cout << ERR << std::endl << ERR_N_PREFIX "Non " ORANGE "print" WHITE "able value(\'" UNDERLINE << printValue << WHITE "\');" << std::endl;
                isValid = false;
            }
        } else {
            std::cout << ERR << std::endl << ERR_N_PREFIX "top value(\'" UNDERLINE
                << (*(_operands->begin()))->toString() << WHITE "\') has non " RED "print" WHITE "able type(bigger than int8 aka char);" << std::endl;
            isValid = false;
        }
    }
    return isValid;
}

bool LaunchAVM::parseDump() {
    if (!_operands->size()) {
        std::cout << ERR_N_PREFIX "any values currently pushed, \'dump\' can't print all stack values;" << std::endl;
        return false;
    } else {
        size_t elementNumber = 0;
        std::list<IOperand const*>::iterator it = _operands->begin();
        while (_operands->end() != it) {
            ++elementNumber;
            std::cout << std::setiosflags(std::ios::right) << " [" UNDERLINE << std::setw(6) << elementNumber << WHITE "]: " << (*it)->toString();
            if (Int32 < (*it)->getType()) {
                std::cout << ", precision = " << (*it)->getPrecision();
            }
            std::cout << ';' << std::endl;
            ++it;
        }
    }
    return true;
}

bool LaunchAVM::baseArithmetic(std::string const command, char const op) {
    if (MIN_OPERANDS_FOR_ARIPHMETHICS > _operands->size()) {
        std::cout << ERR_N_PREFIX "can't parse \'"
            << command << "\' because at the top of the stack less then " UNDERLINE
            << MIN_OPERANDS_FOR_ARIPHMETHICS << WHITE " values;" << std::endl;
        return false;
    } else {
        IOperand const *lOperand = *(_operands->begin());
        IOperand const *rOperand = *(++_operands->begin());
        IOperand const *result = NULL;

        std::cout << " " ORANGE << command << WHITE ": " << (*lOperand).toString()
            << " " ORANGE << op << WHITE " " << (*rOperand).toString() << " = ";
        switch (op) {
            case '+': result = *lOperand + *rOperand; break;
            case '-': result = *lOperand - *rOperand; break;
            case '*': result = *lOperand * *rOperand; break;
            case '/': result = *lOperand / *rOperand; break;
            case '%': result = *lOperand % *rOperand; break;
            default: break;
        }

        if (result) {
            std::cout << '\'' << (*result).toString() << "\';" << std::endl;
            delete lOperand;
            delete rOperand;
            _operands->pop_front();
            _operands->pop_front();
            _operands->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX "something went wrong when parsing \'" << command << "\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool LaunchAVM::parseAdd() { return baseArithmetic("add", '+'); }
bool LaunchAVM::parseSub() { return baseArithmetic("sub", '-'); }
bool LaunchAVM::parseMul() { return baseArithmetic("mul", '*'); }
bool LaunchAVM::parseDiv() { return baseArithmetic("div", '/'); }
bool LaunchAVM::parseMod() { return baseArithmetic("mod", '%'); }

void LaunchAVM::displayUnexecutedCommands(std::vector<std::string> *commandQueue, std::vector<std::string>::iterator &it, size_t commandsCounter) {
    std::cout << WARN_PREFIX "at least [" UNDERLINE << std::setw(6) << (std::distance(commandQueue->begin(),
        std::find_if(it, commandQueue->end(), [](std::string const &str){ return str == "exit"; })) - commandsCounter)
        << WHITE "] commands was un-executed after \'exit\':" << std::endl;

    bool isBreak = false;
    std::vector<std::string>::iterator i = commandQueue->begin();
    while (commandQueue->end() != i) {
        std::cout << " [" UNDERLINE << std::setw(6) << std::distance(commandQueue->begin(), i) + 1 << WHITE "]: ";
        if (i < it) {
            std::cout << DIM CYAN;
        } else {
            if (*i == "exit") {
                std::cout << DIM MAGENTA;
                isBreak = true;
            } else {
                std::cout << BLUE;
            }
        }
        std::cout << *i++ << WHITE ";" << std::endl;
        if (isBreak) { break ; }
    }
}
