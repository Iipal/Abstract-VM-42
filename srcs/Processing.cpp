#include "Processing.hpp"

Processing::Processing() { }
Processing::Processing(const Processing &copy) { *this = copy; }
Processing::~Processing() { }

Processing &Processing::operator=(const Processing &copy) {
    if (this != &copy) { *this = copy; }
    return *this;
}

bool Processing::startProcessing(std::vector<std::string> *commandQueue) {
    std::cout << "    AVM " GREEN "start" WHITE " executing:" << std::endl;
    std::list<IOperand const *> operands;

    bool isValid = true;
    bool _exit = false;
    size_t commandsCounter = 0;
    std::vector<std::string>::iterator it = commandQueue->begin();
    while (!_exit && commandQueue->end() != it) {
        ++commandsCounter;
        for (size_t i = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++i;) {
            if (!(*it).compare(0, _validCommandsWithParams[i].length(), _validCommandsWithParams[i])) {
                if (!(this->*fnptrsCommandsWParam[i])((*it).substr(_validCommandsWithParams[i].length() + 1, (*it).length() - 2), &operands)) {
                    isValid = false;
                }
                break ;
            }
        }

        for (size_t i = ~0ULL; MAX_VALID_NO_PARAM_COMMANDS > ++i;) {
            if (*it == _validCommandsNoParams[i]) {
                if (i == 1) {
                    _exit = true;
                } else {
                    if (!(this->*fnptrsCommandsNoParam[i])(&operands)) {
                        isValid = false;
                    }
                }
                break ;
            }
        }
        ++it;
    }

    if (isValid && _exit) {
        if (commandsCounter != commandQueue->size()) {
            std::cout << WARN_PREFIX "at least [" UNDERLINE << std::setw(6) << (std::distance(commandQueue->begin(),
                std::find_if(it, commandQueue->end(), [](std::string const &str){ return str == "exit"; })) - commandsCounter)
                << WHITE "] commands was un-executed after \'exit\':" << std::endl;
            std::vector<std::string>::iterator i = commandQueue->begin();
            while (commandQueue->end() != i) {
                if (i < it) {
                    std::cout << "[" UNDERLINE << std::setw(6) << std::distance(commandQueue->begin(), i) + 1 << WHITE "] \'" DIM << (*i) << WHITE "\';" << std::endl;
                } else {
                    if ((*i) == "exit") {
                        std::cout << "[" UNDERLINE << std::setw(6) << std::distance(commandQueue->begin(), i) + 1 << WHITE "] \'" DIM RED << (*i) << WHITE "\';" << std::endl;
                        break ;
                    } else {
                        std::cout << "[" UNDERLINE << std::setw(6) << std::distance(commandQueue->begin(), i) + 1 << WHITE "] \'" CHERRY << (*i) << WHITE "\';" << std::endl;
                    }
                }
                ++i;
            }
        } else {
            std::cout << " " UNDERLINE GREEN "successful" WHITE " executed AVM;" << std::endl;
        }
    } else if (isValid && !_exit) {
        std::cout << WARN_PREFIX "executing was stopped without \'" CYAN "exit" WHITE "\';" << std::endl;
    } else {
        std::cout << CYAN "AVM" WHITE " " MAGENTA "work-report" WHITE "   : at least [" RED UNDERLINE
            << std::setw(6) << Reader::getGlobalErrorsCounter()
            << WHITE "] error occured while AVM was executed,"
            " try to fix all error reports above for successful AVM work;" << std::endl;
    }
    if (operands.size()) {
        std::list<IOperand const *>::iterator it = operands.begin();
        while (operands.end() != it) {
            delete *it;
            ++it;
        }
    }
    return isValid;
}

/* private methods */

bool Processing::processPush(std::string const &param, std::list<IOperand const*> *const o) {
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

bool Processing::processAssert(std::string const &param, std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "command queue is empty now, can't process \'assert\';" << std::endl;
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

    std::list<IOperand const*>::const_iterator it = o->begin();
    std::string _itParamValue = (*it)->toString();
    _itParamValue = _itParamValue.substr(_itParamValue.find_first_of('(', 0) + 1,
        _itParamValue.find_first_of(')', 0) - (_itParamValue.find_first_of('(', 0) + 1));
    if ((*it)->getType() != _oType || _itParamValue != _paramValue) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) << UNDERLINE "assert "
            << param << WHITE " is " CHERRY "false" WHITE ";" << std::endl;
        return false;
    } else {
        std::cout << UNDERLINE "assert " << param << WHITE " is " GREEN "true" WHITE ";" << std::endl;
    }
    return true;
}

bool Processing::processPrint(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'print\' can't display top value;" << std::endl;
        return false;
    } else {
        std::cout << "\'print\' top value: \'" << (*(o->begin()))->toString() << "\';" << std::endl;
    }
    return true;
}

bool Processing::processExit(std::list<IOperand const*> *const o) { (void)o; return true; }

bool Processing::baseProcessAriphmetic(std::list<IOperand const *> *const o, std::string const command, char const op) {
    if (2 > o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "can't process \'"
            << command << "\' because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        IOperand const *result = NULL;

        std::cout << "\'" << (*leftOperand).toString() << "\' " CYAN << op << WHITE " \'" << (*rightOperand).toString() << "\' = ";
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
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'" << command << "\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool Processing::processAdd(std::list<IOperand const*> *const o) { return baseProcessAriphmetic(o, "add", '+'); }
bool Processing::processSub(std::list<IOperand const*> *const o) { return baseProcessAriphmetic(o, "sub", '-'); }
bool Processing::processMul(std::list<IOperand const*> *const o) { return baseProcessAriphmetic(o, "mul", '*'); }
bool Processing::processDiv(std::list<IOperand const*> *const o) { return baseProcessAriphmetic(o, "div", '/'); }
bool Processing::processMod(std::list<IOperand const*> *const o) { return baseProcessAriphmetic(o, "mod", '%'); }

bool Processing::processPop(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'pop\' can't unstack value from top;" << std::endl;
        return false;
    } else {
        std::cout << "pop \'" CHERRY << (*(o->begin()))->toString() << WHITE "\' value from the top of the stack;" << std::endl;
        delete *(o->begin());
        o->pop_front();
    }
    return true;
}

bool Processing::processDump(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'dump\' can't print all stack values;" << std::endl;
        return false;
    } else {
        size_t elementNumber = ~0ULL;
        std::list<IOperand const*>::const_iterator it = o->begin();
        while (o->end() != it) {
            std::cout << std::setiosflags(std::ios::right) << "[" UNDERLINE << std::setw(6)
                << ++elementNumber << WHITE "] - precision \'" UNDERLINE << std::setw(3) << (*it)->getPrecision() << WHITE "\', " << (*it)->toString() << ';' << std::endl;
            ++it;
        }
    }
    return true;
}
