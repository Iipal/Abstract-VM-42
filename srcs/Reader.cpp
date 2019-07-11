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

    const size_t avmDefaultInputMsgLenght = 1 + _hostName.length() + _fullExecutablePath.length();

    bool isAllValid = true;
    std::cout << INVERT "   " << std::setiosflags(std::ios::left) << std::setw(avmDefaultInputMsgLenght) << "AVM standard console input mode" << WHITE << std::endl;
    std::string _tmp;
    bool _exit = false;
    while (!_exit) {
        std::cout << RED "@" WHITE << _hostName << " " RED "➜" WHITE " " << INVERT << _fullExecutablePath << WHITE << ": ";
        std::getline(std::cin, _tmp);
        if (_tmp == ";;") {
            _exit = true;
            std::cout << INVERT "   " << std::setiosflags(std::ios::left) << std::setw(avmDefaultInputMsgLenght) << "Starts AVM Command Queue Parser:" << WHITE << std::endl;
        } else if (std::cin.bad() || std::cin.eof()) {
            _exit = true;
            std::cout << std::endl << ERR_PREFIX "Occured error symbol in standard input;" << std::endl;
            isAllValid = false;
        } else {
            if (_tmp.size()) {
                if (_tmp == "help" || _tmp == "h") {
                    std::cout << INVERT "   " << std::setiosflags(std::ios::left) << std::setw(avmDefaultInputMsgLenght) << "AVM help info: " << WHITE << std::endl << std::setiosflags(std::ios::left)
                        << "| command | " << std::setw(14) << "parameter" << std::setw(95) << ": description" << '|' << std::endl
                        << "| " INVERT "exit   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stop to execute command queue and exit from AVM (necessary at the end of command queue);" << '|' << std::endl
                        << "| " INVERT "print  " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Asserts that the value at the top of the stack is an 8-bit integer;" << '|' << std::endl
                        << "| " INVERT "assert " WHITE " | " << std::setw(14) << "@exception"    << std::setw(95) << ": check is @exception is true or not;" << '|' << std::endl
                        << "| " INVERT "push   " WHITE " | " << std::setw(14) << "@type(@value)" << std::setw(95) << ": valid @type is int8, int16, int32, float, double; Pushes the @value at the top of the stack;" << '|' << std::endl
                        << "| " INVERT "pop    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the value from the top of the stack;" << '|' << std::endl
                        << "| " INVERT "add    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, adds them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "sub    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, subtracts them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "mul    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, multiplies them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "div    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, divides them, then stacks the result;" << '|' << std::endl
                        << "| " INVERT "mod    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, calculates the modulus, then stacks the result;" << '|' << std::endl
                        << "| " INVERT ";;     " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stop waiting for any input, start syntax command queue parse and try to execute AVM;" << '|' << std::endl
                        << "| " INVERT "help   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Print this help info;" << '|' << std::endl;
                } else {
                    outCommandsQueue->push_back(_tmp);
                }
            }
        }
    }

    if (!outCommandsQueue->size()) {
        std::cout << ERR_PREFIX << "command queue is empty, can't execute AVM." << std::endl;
        isAllValid = false;
    }
    if (!isAllValid) {
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
    const std::string _validCommandsNoParams[MAX_VALID_NO_PARAM_COMMANDS] = { "print", "exit",
                                                            "add", "sub", "mul", "div", "mod",
                                                            "pop", "dump" };
    const std::string _validCommandsWithParams[MAX_VALID_W_PARAM_COMMANDS] = { "push ", "assert " };

    bool isValidCurrentCommand = false;
    bool isValidCommandQueue = true;

    size_t i = ~0ULL;
    while (commandQueue->size() > ++i) {
        isValidCurrentCommand = false;

        { /* validate commands what hasn't any parameters. */
            size_t j = ~0ULL;
            while (MAX_VALID_NO_PARAM_COMMANDS > ++j) {
                if ((*commandQueue)[i] == _validCommandsNoParams[j]) {
                    isValidCurrentCommand = true;
                }
            }
        }
        { /* validate commands what has parameter. */
            size_t j = ~0ULL;
            while (MAX_VALID_W_PARAM_COMMANDS > ++j && !isValidCurrentCommand) {
                if (!(*commandQueue)[i].compare(0, _validCommandsWithParams[j].length(), _validCommandsWithParams[j].c_str())) {
                    if (!j) { /* if (!j): means if we trying to validate 'push' command */
                        isValidCurrentCommand
                            = this->validatePushCommand((*commandQueue)[i].substr(_validCommandsWithParams[j].length(),
                                _validCommandsWithParams[j].length() - (*commandQueue)[i].length()));
                    } else {
                        isValidCurrentCommand = true;
                    }
                }
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
                    return isValid;
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
