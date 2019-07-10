#include "Reader.hpp"

Reader::Reader() { }
Reader::Reader(Reader const &copy) { *this = copy; }
Reader::~Reader() { }

Reader &Reader::operator=(Reader const &copy) {
    if (this != &copy) { *this = copy; }
    return *this;
}

std::vector<std::string> *Reader::readStandardInput(void) const {
    std::vector<std::string> *outCommandsQueue = new std::vector<std::string>();

    std::string _tmp;
    while (std::getline(std::cin, _tmp)) {
        if (_tmp != ";;") {
            if (_tmp != "") {
                outCommandsQueue->push_back(_tmp);
            }
        } else {
            break ;
        }
    }

    if (!outCommandsQueue->size()) {
        std::cout << ERR_PREFIX << "command queue is empty, can't execute AVM." << std::endl;
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    }

    return outCommandsQueue;
}

std::vector<std::string> *Reader::readFileInput(std::string const &fileName) const {
    std::vector<std::string> *outCommandsQueue = new std::vector<std::string>();

    std::fstream _file(fileName);
    if (_file.is_open()) {
        std::string _tmp;
        while (std::getline(_file, _tmp)) {
            if (_tmp.compare(0, 1, ";")) {
                if (_tmp != "") {
                    outCommandsQueue->push_back(_tmp);
                }
            }
        }

        if (!outCommandsQueue->size()) {
            std::cout << ERR_PREFIX << "command queue is empty, can't execute AVM." << std::endl;
            delete outCommandsQueue;
            outCommandsQueue = NULL;
        }
    } else  {
        std::cout << "ERROR: Invalid file." << std::endl;
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    }
    return outCommandsQueue;
}

bool Reader::validatingReadedCommandQueue(std::vector<std::string> *commandQueue) const {
    const std::string _validCommandsWithParams[MAX_VALID_W_PARAM_COMMANDS] = { "push", "assert" };
    const std::string _validCommandsNoParams[MAX_VALID_NO_PARAM_COMMANDS] = { "print", "exit",
                                                    "add", "sub", "mul", "div", "mod",
                                                    "pop", "dump" };
    bool isValidCurrentCommand = false;
    bool isValidCommandQueue = true;

    size_t i = ~0ULL;
    while (commandQueue->size() > ++i) {
        isValidCurrentCommand = false;

        size_t j = ~0ULL;
        while (MAX_VALID_NO_PARAM_COMMANDS > ++j) {
            if ((*commandQueue)[i] == _validCommandsNoParams[j]) {
                std::cout << (*commandQueue)[i] << std::endl;
                isValidCurrentCommand = true;
            }
        }
        j = ~0ULL;
        while (MAX_VALID_W_PARAM_COMMANDS > ++j) {
            if (!(*commandQueue)[i].compare(0, _validCommandsWithParams[j].length(), _validCommandsWithParams[j].c_str()))  {
                std::cout << (*commandQueue)[i] << std::endl;
                isValidCurrentCommand = true;
            }
        }

        if (false == isValidCurrentCommand) {
            std::cout << ERR_PREFIX << '\'' << (*commandQueue)[i] << "' is an invalid command;" << std::endl;
            isValidCommandQueue = false;
        }
    }
    if (false == isValidCommandQueue) {
        std::cout << ERR_PREFIX << "invalid command queue, execute AVM is impossible;" << std::endl;
    }
    return isValidCommandQueue;
}
