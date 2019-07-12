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
    if (!outCommandsQueue) {
        std::cout << ERR_PREFIX "Cannot allocate memory." << std::endl;
        return outCommandsQueue;
    }

    std::string _hostName;
    std::string _fullExecutablePath;
    {
        char _hnBuff[32];
        gethostname(_hnBuff, 32);

        char *_fepBuff = NULL;
        _fepBuff = getcwd(_fepBuff, 64);

        _fullExecutablePath = std::string(_fepBuff);
        _hostName = std::string(_hnBuff);
    }

    const size_t avmDefaultInputMsgLenght = _hostName.length() + _fullExecutablePath.length();

    std::cout << INVERT "    " << std::setiosflags(std::ios::left) << std::setw(avmDefaultInputMsgLenght) << "AVM CIn mode ('h' for details)" << WHITE << std::endl;

    bool isValid = true;
    std::string _tmp;
    bool _exit = false;
    while (!_exit) {
        std::cout << RED "@" WHITE << _hostName << " " RED "➜" WHITE " " INVERT << _fullExecutablePath << WHITE ": ";
        std::getline(std::cin, _tmp);
        if (_tmp == ";;") {
            _exit = true;
        } else if (std::cin.bad() || std::cin.eof() || std::cin.fail()) {
            _exit = true;
            std::cout << std::endl << ERR_PREFIX "Error occured in standard input;" << std::endl;
            isValid = false;
        } else {
            if (_tmp.size()) {
                if (_tmp == "help" || _tmp == "h") {
                    std::cout << INVERT "    " << std::setiosflags(std::ios::left) << std::setw(avmDefaultInputMsgLenght) << "AVM help info: " << WHITE << std::endl << std::setiosflags(std::ios::left)
                        << "| "        "command"       " | " << std::setw(14) << "parameter"     << std::setw(95) << ": description" << '|' << std::endl
                        << "| " INVERT "exit   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stop to execute command queue and exit from AVM (necessary at the end of command queue);" << '|' << std::endl
                        << "| " INVERT "print  " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Asserts that the value at the top of the stack is an 8-bit integer;" << '|' << std::endl
                        << "| " INVERT "assert " WHITE " | " << std::setw(14) << "@exception"    << std::setw(95) << ": check if @exception is true or not;" << '|' << std::endl
                        << "| " INVERT "push   " WHITE " | " << std::setw(14) << "@type(@value)" << std::setw(95) << ": valid @type is int8, int16, int32, float, double; Pushes the @value at the top of the stack;" << '|' << std::endl
                        << "| " INVERT "pop    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the value from the top of the stack;" << '|' << std::endl
                        << "| " INVERT "add    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, adds them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "sub    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, subtracts them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "mul    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, multiplies them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "div    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, divides them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "mod    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, calculates the modulus, then stacks the result;" << '|' << std::endl
                        << "| " INVERT ";;     " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stop waiting for any input and try to execute AVM;" << '|' << std::endl
                        << "| " INVERT "help/h " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Print this help info;" << '|' << std::endl
                        << "| " INVERT "       " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Empty lines will be ignored;" << '|' << std::endl
                        << "| " INVERT "quit/q " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stop input and quit from AVM;" << '|' << std::endl;
                } else if (_tmp == "quit" || _tmp == "q") {
                    _exit = true;
                    isValid = false;
                } else {
                    if (validatingReadedCommand(_tmp)) {
                        outCommandsQueue->push_back(_tmp);
                    } else {
                        std::cout << ERR_PREFIX "invalid command was detected, it's was ignored to add to command queue, try another command ('h')" << std::endl;
                    }
                }
            }
        }
    }

    if (isValid && !outCommandsQueue->size()) {
        std::cout << ERR_PREFIX "command queue is empty, can't execute AVM." << std::endl;
        isValid = false;
    }
    if (!isValid) {
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    }
    return outCommandsQueue;
}

std::vector<std::string> *Reader::readFileInput(std::string const &fileName) const {
    std::vector<std::string> *outCommandsQueue = new std::vector<std::string>();
    if (!outCommandsQueue) {
        std::cout << ERR_PREFIX "Cannot allocate memory." << std::endl;
        return outCommandsQueue;
    }

    bool isValid = true;
    std::fstream _file(fileName);
    if (_file.is_open()) {
        std::string _tmp;
        while (std::getline(_file, _tmp)) {
            if (_tmp.compare(0, 1, ";")) {
                if (_tmp.size()) {
                    if (validatingReadedCommand(_tmp)) {
                        outCommandsQueue->push_back(_tmp);
                    } else {
                        isValid = false;
                    }
                }
            }
        }
        if (!outCommandsQueue->size()) {
            std::cout << ERR_PREFIX << "command queue is empty, can't execute AVM." << std::endl;
            isValid = false;
        }
    } else  {
        std::cout << ERR_PREFIX "invalid file." << std::endl;
        isValid = false;
    }
    if (false == isValid) {
        std::cout << ERR_PREFIX << "invalid command queue. Cann't execute AVM." << std::endl;
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    }
    return outCommandsQueue;
}

bool Reader::validatingReadedCommand(std::string const &command) const {
    static const std::string _validCommandsNoParams[MAX_VALID_NO_PARAM_COMMANDS] = { "print", "exit",
                                                            "add", "sub", "mul", "div", "mod",
                                                            "pop", "dump" };
    static const std::string _validCommandsWithParams[MAX_VALID_W_PARAM_COMMANDS] = { "push ", "assert " };

    bool isValidCurrentCommand = false;

    for (size_t j = ~0ULL; MAX_VALID_NO_PARAM_COMMANDS > ++j;) {
        if (command == _validCommandsNoParams[j]) {
            isValidCurrentCommand = true;
        }
    }

    for (size_t j = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++j;) {
        if (!command.compare(0, _validCommandsWithParams[j].length(), _validCommandsWithParams[j].c_str())) {
            if (!j) { /* if (!j): means if we trying to validate 'push' command */
                isValidCurrentCommand
                    = this->validatePushCommand(command.substr(_validCommandsWithParams[j].length(),
                        _validCommandsWithParams[j].length() - command.length()));
            } else {
                isValidCurrentCommand = true;
            }
        }
    }

    if (false == isValidCurrentCommand) {
        std::cout << ERR_PREFIX "\'" << command << "\' is an invalid command or miss parameter;" << std::endl;
    }
    return isValidCurrentCommand;
}

/* private methods */
bool Reader::validatePushCommand(std::string const &_pushType) const {
    static const std::string _validPushParamTypes[MaxOperandTypes] = { "int8(", "int16(", "int32(", "float(", "double(" };

    bool isValid = true;
    size_t i = ~0ULL;
    while (MaxOperandTypes > ++i) {
        if (!_pushType.compare(0, _validPushParamTypes[i].length(), _validPushParamTypes[i].c_str())) {
            std::string _pushValue = _pushType.substr(_validPushParamTypes[i].length(), _pushType.length() - _validPushParamTypes[i].length() - 1);
            const size_t _pushValueTypeParamEndBracketPos = _pushType.find_first_of(')', _validPushParamTypes[i].length());

            if (_pushValueTypeParamEndBracketPos < _pushType.length() && _pushValueTypeParamEndBracketPos + 1 == _pushType.length()) {
                if (3 > i) {
                    isValid = !_pushValue.empty()
                        && std::find_if(_pushValue.begin(), _pushValue.end(), [](char c) { return !std::isdigit(c); }) == _pushValue.end();
                    if (!isValid) {
                        std::cout << ERR_PREFIX << "\'" << _pushValue << "\' must to contain only digits;" << std::endl;
                    }
                } else {
                    std::cout << _pushValue << std::endl;
                }
            } else {
                if (_pushValueTypeParamEndBracketPos < _pushType.length() && _pushValueTypeParamEndBracketPos + 1 != _pushType.length()) {
                    std::cout << ERR_PREFIX << "trash detected after ending bracket: \'" << _pushType.substr(_pushValueTypeParamEndBracketPos + 1, _pushType.length() - _pushValueTypeParamEndBracketPos) << "\'; " << std::endl;
                } else {
                    std::cout << ERR_PREFIX << "in \'" << _pushType << "\' has no ending bracket \')\'" << std::endl;
                }
                isValid = false;
            }
            return isValid;
        }
    }

    return false;
}
