#include "AVMLaunchProcessing.hpp"

AVMLaunchProcessing::AVMLaunchProcessing() { }
AVMLaunchProcessing::AVMLaunchProcessing(const AVMLaunchProcessing &copy) { *this = copy; }
AVMLaunchProcessing::~AVMLaunchProcessing() { }

AVMLaunchProcessing &AVMLaunchProcessing::operator=(const AVMLaunchProcessing &copy) {
    if (this != &copy) { *this = copy; }
    return *this;
}

bool AVMLaunchProcessing::startProcessing(std::vector<std::string> *commandQueue) {
    std::list<IOperand const*> operands;

    bool isValid = true;
    bool _exit = false;
    size_t commandsCounter = 0;
    std::vector<std::string>::const_iterator it = commandQueue->begin();
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
        std::cout << " " UNDERLINE GREEN "successful" WHITE " executed AVM;" << std::endl;
        if (commandsCounter != commandQueue->size()) {
            size_t i = commandsCounter;
            while (commandQueue->size() > i) {
                if ((*commandQueue)[i] == "exit") { break ; } else { ++i; }
            }
            std::cout << WARN_PREFIX "at least [" UNDERLINE << std::setw(6) << (i - commandsCounter)
                << WHITE "] commands was un-executed after \'exit\':" << std::endl;
            i = commandsCounter;
            while (commandQueue->size() > i) {
                if ((*commandQueue)[i] == "exit") {
                    break ;
                } else {
                    std::cout << "[" UNDERLINE << std::setw(6) << i + 1 << WHITE "] \'" MAGENTA << (*commandQueue)[i] << WHITE "\';" << std::endl;
                    ++i;
                }
            }
        }
    } else if (isValid && !_exit) {
        std::cout << WARN_PREFIX "executing was stopped without \'" CYAN "exit" WHITE "\';" << std::endl;
    } else {
        std::cout << CYAN "AVM" WHITE " " MAGENTA "work-report" WHITE "   : at least [" RED UNDERLINE
            << std::setw(6) << Reader::getGlobalErrorsCounter()
            << WHITE "] error occured while AVM was executed,"
            " try to fix all error reports above for successful AVM work;" << std::endl;
    }
    return isValid;
}

/* private methods */

bool AVMLaunchProcessing::processPush(std::string const &param, std::list<IOperand const*> *const o) {
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

bool AVMLaunchProcessing::processAssert(std::string const &param, std::list<IOperand const*> *const o) {
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

bool AVMLaunchProcessing::processPrint(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'print\' can't display top value;" << std::endl;
        return false;
    } else {
        std::cout << "\'print\' top value: \'" << (*(o->begin()))->toString() << "\';" << std::endl;
    }
    return true;
}

bool AVMLaunchProcessing::processExit(std::list<IOperand const*> *const o) { (void)o; return true; }

bool AVMLaunchProcessing::processAdd(std::list<IOperand const*> *const o) {
    if  (2 > o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "can't process \'add\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' " MAGENTA "add" WHITE " \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand + *rightOperand;
        if (result) {
            std::cout << "\'" GREEN << (*result).toString() << WHITE "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'add\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool AVMLaunchProcessing::processSub(std::list<IOperand const*> *const o) {
    if (2 > o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "can't process \'sub\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' " MAGENTA "sub" WHITE " \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand - *rightOperand;
        if (result) {
            std::cout << "\'" GREEN << (*result).toString() << WHITE "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'sub\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool AVMLaunchProcessing::processMul(std::list<IOperand const*> *const o) {
    if (2 > o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "can't process \'mul\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' " MAGENTA "mul" WHITE " \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand * *rightOperand;
        if (result) {
            std::cout << "\'" GREEN << (*result).toString() << WHITE "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'mul\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool AVMLaunchProcessing::processDiv(std::list<IOperand const*> *const o) {
    if (2 > o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "can't process \'div\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' " MAGENTA "div" WHITE " \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand / *rightOperand;
        if (result) {
            std::cout << "\'" GREEN << (*result).toString() << WHITE "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'div\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool AVMLaunchProcessing::processMod(std::list<IOperand const*> *const o) {
    if (2 > o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "can't process \'mod\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' " MAGENTA "mod" WHITE " \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand % *rightOperand;
        if (result) {
            std::cout << "\'" GREEN << (*result).toString() << WHITE "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'mod\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool AVMLaunchProcessing::processPop(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'pop\' can't unstack value from top;" << std::endl;
        return false;
    } else {
        std::cout << "pop \'" MAGENTA << (*(o->begin()))->toString() << WHITE "\' value from the top of the stack;" << std::endl;
        o->pop_front();
    }
    return true;
}

bool AVMLaunchProcessing::processDump(std::list<IOperand const*> *const o) {
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
