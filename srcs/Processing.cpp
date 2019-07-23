#include "Processing.hpp"

Processing::Processing() { }
Processing::Processing(const Processing &copy) { *this = copy; }
Processing::~Processing() { if (!isClear()) { clear(); } }

Processing &Processing::operator=(const Processing &copy) {
    if (this != &copy) {
        this->_operands = NULL;
        this->_isClear = true;
    }
    return *this;
}

bool Processing::startProcessing(std::vector<std::string> *commandQueue) {
    Reader::refreshGlobalErrorsCounter();

    _operands = new std::list<IOperand const*>();
    if (!_operands) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "can't allocate memory;" << std::endl;
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
            << std::setw(6) << Reader::getGlobalErrorsCounter()
            << WHITE "] error occured while AVM was executed, try to fix all error reports above for successful AVM work;" << std::endl;
    }
    clear();
    return isValid;
}

/* private methods */

bool const &Processing::isClear() const { return this->_isClear; }

void Processing::clear() {
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

bool Processing::processPush(std::string const &param) {
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

    _operands->push_front(gOFactory.createOperand(_oType, _paramValue));
    std::cout << " push \'" BLUE << _paramType << "(" << _paramValue << ")" WHITE "\';" << std::endl;
    return true;
}

bool Processing::processAssert(std::string const &param) {
    if (!_operands->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "any values currently pushed, can't process \'assert\';" << std::endl;
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
    std::string _itParamValue = (*it)->toString();
    _itParamValue = _itParamValue.substr(_itParamValue.find_first_of('(', 0) + 1,
        _itParamValue.find_first_of(')', 0) - (_itParamValue.find_first_of('(', 0) + 1));
    if ((*it)->getType() != _oType || _itParamValue != _paramValue) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            << param << " is " RED "false" WHITE ";" << std::endl;
        return false;
    } else {
        std::cout << param << " is " GREEN "true" WHITE ";" << std::endl;
    }
    return true;
}

bool Processing::processExit() { return true; }

bool Processing::processPop() {
    if (!_operands->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "any values currently pushed, \'pop\' can't unstack value from top;" << std::endl;
        return false;
    } else {
        std::cout << " pop \'" RED << (*(_operands->begin()))->toString() << WHITE "\';" << std::endl;
        delete *(_operands->begin());
        _operands->pop_front();
    }
    return true;
}

void Processing::baseDisplayOperand(IOperand const *it, size_t i) {
    std::cout << std::setiosflags(std::ios::right) << "[" UNDERLINE << std::setw(6) << i << WHITE "]: " << it->toString();
    if (Int32 < it->getType()) {
        std::cout << ", precision = " << it->getPrecision();
    }
    std::cout << ';' << std::endl;
}

bool Processing::processPrint() {
    if (!_operands->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "any values currently pushed, \'print\' can't display top value;" << std::endl;
        return false;
    } else {
        baseDisplayOperand(*(_operands->begin()), 1);
    }
    return true;
}

bool Processing::processDump() {
    if (!_operands->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "any values currently pushed, \'dump\' can't print all stack values;" << std::endl;
        return false;
    } else {
        size_t elementNumber = ~0ULL;
        std::list<IOperand const*>::const_iterator it = _operands->begin();
        while (_operands->end() != it) {
            baseDisplayOperand(*it++, ++elementNumber + 1);
        }
    }
    return true;
}

bool Processing::baseProcessAriphmetic(std::string const command, char const op) {
    if (2 > _operands->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "can't process \'"
            << command << "\' because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(_operands->begin());
        IOperand const *rightOperand = *(++_operands->begin());
        IOperand const *result = NULL;

        std::cout << "\'" << (*leftOperand).toString() << "\' " BLUE << op << WHITE " \'" << (*rightOperand).toString() << "\' = ";
        switch (op) {
            case '+': result = *leftOperand + *rightOperand; break;
            case '-': result = *leftOperand - *rightOperand; break;
            case '*': result = *leftOperand * *rightOperand; break;
            case '/': result = *leftOperand / *rightOperand; break;
            case '%': result = *leftOperand % *rightOperand; break;
            default: break;
        }

        if (result) {
            std::cout << "\'" UNDERLINE << (*result).toString() << WHITE "\';" << std::endl;
            delete leftOperand; delete rightOperand;
            _operands->pop_front(); _operands->pop_front();
            _operands->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'" << command << "\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool Processing::processAdd() { return baseProcessAriphmetic("add", '+'); }
bool Processing::processSub() { return baseProcessAriphmetic("sub", '-'); }
bool Processing::processMul() { return baseProcessAriphmetic("mul", '*'); }
bool Processing::processDiv() { return baseProcessAriphmetic("div", '/'); }
bool Processing::processMod() { return baseProcessAriphmetic("mod", '%'); }

void Processing::displayUnexecutedCommands(std::vector<std::string> *commandQueue, std::vector<std::string>::iterator &it, size_t const &commandsCounter) {
    std::cout << WARN_PREFIX "at least [" UNDERLINE << std::setw(6) << (std::distance(commandQueue->begin(),
        std::find_if(it, commandQueue->end(), [](std::string const &str){ return str == "exit"; })) - commandsCounter)
        << WHITE "] commands was un-executed after \'exit\':" << std::endl;

    bool isBreak = false;
    std::vector<std::string>::iterator i = commandQueue->begin();
    while (commandQueue->end() != i) {
        std::cout << "[" UNDERLINE << std::setw(6) << std::distance(commandQueue->begin(), i) + 1 << WHITE "]: \'";
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
        std::cout << *i++ << WHITE "\';" << std::endl;
        if (isBreak) { break ; }
    }
}
