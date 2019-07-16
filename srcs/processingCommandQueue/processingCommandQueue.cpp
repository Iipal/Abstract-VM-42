#include "processingCommandQueue.hpp"

bool processingCommandQueue(std::vector<std::string> *commandQueue) {
    std::list<IOperand const*> operands;

    bool isValid = true;
    bool _exit = false;
    std::vector<std::string>::const_iterator it = commandQueue->begin();
    while (!_exit && commandQueue->end() != it) {
        for (size_t i = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++i;) {
            if (!(*it).compare(0, _validCommandsWithParams[i].length(), _validCommandsWithParams[i])) {
                isValid = fnptrsCommandsWParam[i]((*it).substr(_validCommandsWithParams[i].length() + 1, (*it).length() - 2), &operands);
                break ;
            }
        }

        for (size_t i = ~0ULL; MAX_VALID_NO_PARAM_COMMANDS > ++i;) {
            if (*it == _validCommandsNoParams[i]) {
                if (*it == "exit") {
                    _exit = isValid = true;
                } else {
                    isValid = fnptrsCommandsNoParam[i](&operands);
                }
                break ;
            }
        }

        ++it;
    }

    if (isValid && _exit) {
        std::cout << "  " GREEN "SUCCESSFUL" WHITE " executed AVM;" << std::endl;
    } else if (!_exit) {
        std::cout << WARN_PREFIX "executing was stopped without \'" CYAN "exit" WHITE "\';" << std::endl;
    }
    return isValid;
}
