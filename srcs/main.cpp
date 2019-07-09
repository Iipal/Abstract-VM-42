#include <iostream>
#include <fstream>
#include <vector>

#define MAX_VALID_COMMANDS 11
static const std::string _validCommandQueue[MAX_VALID_COMMANDS] = { "print", "exit",
                                                    "add", "sub", "mul", "div", "mod",
                                                    "push", "pop", "dump", "assert" };

int main(int argc, char *argv[]) {
    --argc; ++argv;

    bool    _isStartAVM = false;
    std::vector<std::string> _commandQueue;

    if (!argc) {
        std::string _tmp;
        while (std::getline(std::cin, _tmp)) {
            if (_tmp != ";;") {
                _commandQueue.push_back(_tmp);
            } else {
                break ;
            }
        }
        _isStartAVM = true;
    } else if (argc == 1) {
        std::fstream _file(*argv);
        if (_file.is_open()) {
            std::string _tmp;
            while (std::getline(_file, _tmp)) {
                if (_tmp.compare(0, 1, ";")) {
                    _commandQueue.push_back(_tmp);
                }
            }
            _isStartAVM = true;
        } else {
            std::cout << "Cann't open " << *argv << "." << std::endl;
        }
    }
    if (_isStartAVM) {
        bool isInvalidCommandQueue = false;
        size_t i = ~0ULL;
        while (_commandQueue.size() > ++i) {
            size_t j = ~0ULL;
            bool isValidCurrentCommand = false;
            while (MAX_VALID_COMMANDS > ++j) {
                if (_commandQueue[i] == _validCommandQueue[j]) {
                    std::cout << _commandQueue[i] << std::endl;
                    isValidCurrentCommand = true;
                }
            }
            if (!isValidCurrentCommand) {
                std::cout << "ERROR: '" << _commandQueue[i] << "' is an invalid command;" << std::endl;
                isInvalidCommandQueue = true;
            }
        }
        if (!isInvalidCommandQueue) {
            std::cout << "start AVM." << std::endl;
        } else {
            std::cout << "commands syntax error." << std::endl;
        }
    }
}
