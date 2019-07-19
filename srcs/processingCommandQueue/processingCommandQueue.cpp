#include "processingCommandQueue.hpp"

bool processingCommandQueue(std::vector<std::string> *commandQueue) {
    std::list<IOperand const*> operands;

    bool isValid = true;
    bool _exit = false;
    std::vector<std::string>::const_iterator it = commandQueue->begin();
    while (!_exit && commandQueue->end() != it) {
        for (size_t i = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++i;) {
            if (!(*it).compare(0, _validCommandsWithParams[i].length(), _validCommandsWithParams[i])) {
                if (!fnptrsCommandsWParam[i]((*it).substr(_validCommandsWithParams[i].length() + 1, (*it).length() - 2), &operands)) {
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
                    if (!fnptrsCommandsNoParam[i](&operands)) {
                        isValid = false;
                    }
                }
                break ;
            }
        }

        ++it;
    }

    if (isValid && _exit) {
        std::cout << "  " GREEN "SUCCESSFUL" WHITE " executed AVM;" << std::endl;
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
