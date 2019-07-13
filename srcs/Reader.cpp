#include "Reader.hpp"

const std::string Reader::_validCommandsNoParams[MAX_VALID_NO_PARAM_COMMANDS] = { "print", "exit",
                                                                "add", "sub", "mul", "div", "mod",
                                                                                    "pop", "dump" };
const std::string Reader::_validCommandsWithParams[MAX_VALID_W_PARAM_COMMANDS] = { "push", "assert" };
const std::string Reader::_validPushParamTypes[MaxOperandTypes] = { "int8", "int16", "int32",
                                                                        "float", "double" };

Reader::Reader() { }
Reader::Reader(Reader const &copy) { *this = copy; }
Reader::~Reader() { }

Reader &Reader::operator=(Reader const &copy) {
    if (this != &copy) { *this = copy; }
    return *this;
}

std::list<std::string> *Reader::readStandardInput(void) const {
    std::list<std::string> *outCommandsQueue = new std::list<std::string>();
    if (!outCommandsQueue) {
        std::cout << ERR_PREFIX "cannot allocate memory;" << std::endl;
        return outCommandsQueue;
    }

    std::string _hostName;
    std::string _fullExecutablePath;
    {
        char _hnBuff[32];
        gethostname(_hnBuff, 32);
        _hostName = std::string(_hnBuff);

        char *_fepBuff = NULL;
        _fepBuff = getcwd(_fepBuff, 64);
        _fullExecutablePath = std::string(_fepBuff);
    }

    std::cout << "    AVM console input mode " RED "('h' for details):" << WHITE << std::endl;

    std::string _tmp;
    bool isValid = true;
    bool _exit = false;
    while (!_exit) {
        std::cout << INVERT BLUE "AVM" WHITE "@" << _hostName << " " RED "➜" WHITE " " INVERT << _fullExecutablePath << WHITE ": ";
        std::getline(std::cin, _tmp);
        if (std::cin.bad() || std::cin.eof() || std::cin.fail()) {
            std::cout << std::endl << ERR_PREFIX "Error occured in standard input;" << std::endl;
            _exit = true;
            isValid = false;
        } else if (_tmp.size()) {
            if (_tmp == ";;") {
                _exit = true;
            } else {
                if (_tmp == "quit" || _tmp == "q") {
                    _exit = true;
                    isValid = false;
                } else if (_tmp == "help" || _tmp == "h") {
                    std::cout << INVERT "    AVM help info:   " << WHITE << std::endl << std::setiosflags(std::ios::left)
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
                        << "| " INVERT ";;     " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stops waiting for any input and execute AVM;" << '|' << std::endl
                        << "| " INVERT "quit/q " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stops waiting for any input and quit without execute AVM;" << '|' << std::endl
                        << "| " INVERT "help/h " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Print this help info;" << '|' << std::endl
                        << "| " INVERT "       " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Empty lines will be ignored;" << '|' << std::endl;
                } else {
                    if (validatingReadedCommand(_tmp)) {
                        outCommandsQueue->push_front(_tmp);
                    } else {
                        std::cout << ERR_PREFIX "invalid command was detected, it's was ignored to add to command queue, try another command ('h');" << std::endl;
                    }
                }
            }
        }
    }

    if (isValid && !outCommandsQueue->size()) {
        std::cout << ERR_PREFIX "command queue is empty, can't execute AVM;" << std::endl;
        isValid = false;
    }
    if (!isValid) {
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    }
    return outCommandsQueue;
}

std::list<std::string> *Reader::readPipeInput(void) const {
    std::list<std::string> *outCommandsQueue = new std::list<std::string>();
    if (!outCommandsQueue) {
        std::cout << ERR_PREFIX "cannot allocate memory;" << std::endl;
        return outCommandsQueue;
    }

    std::cout << "    AVM input from a pipe mode: " << std::endl;

    std::string _tmp;
    bool isValid = true;
    bool _exit = false;
    while (!_exit) {
        if (!std::getline(std::cin, _tmp)) {
            _exit = true;
        } else if (std::cin.bad() || std::cin.eof() || std::cin.fail()) {
            std::cout << std::endl << ERR_PREFIX "error occured in pipe input;" << std::endl;
            _exit = true;
            isValid = false;
        } else if (_tmp.size()) {
            if (_tmp.compare(0, 1, ";")) {
                if (validatingReadedCommand(_tmp)) {
                    outCommandsQueue->push_front(_tmp);
                } else  {
                    isValid = false;
                }
            }
        }
    }
    if (false == isValid) {
        std::cout << ERR_PREFIX "invalid command queue. can't execute AVM;" << std::endl;
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    } else {
        std::cout << GREEN "successful" WHITE " read command queue from a pipe;" << std::endl;
    }
    return outCommandsQueue;
}

std::list<std::string> *Reader::readFileInput(std::string const &fileName) const {
    std::list<std::string> *outCommandsQueue = new std::list<std::string>();
    if (!outCommandsQueue) {
        std::cout << ERR_PREFIX "cannot allocate memory;" << std::endl;
        return outCommandsQueue;
    }

    std::cout << "    AVM input from a file mode: " << std::endl;

    bool isValid = true;
    std::fstream _file(fileName);
    if (_file.is_open()) {
        std::string _tmp;
        while (std::getline(_file, _tmp)) {
            if (_tmp.size()) {
                if (_tmp.compare(0, 1, ";")) {
                    if (validatingReadedCommand(_tmp)) {
                        outCommandsQueue->push_front(_tmp);
                    } else {
                        isValid = false;
                    }
                }
            }
        }
        if (isValid && !outCommandsQueue->size()) {
            std::cout << ERR_PREFIX "command queue is empty, can't execute AVM." << std::endl;
            isValid = false;
        }
    } else {
        std::cout << ERR_PREFIX "\'" << fileName << "\' is an invalid file." << std::endl;
        isValid = false;
    }
    if (false == isValid) {
        std::cout << ERR_PREFIX "invalid command queue. Can't execute AVM." << std::endl;
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    } else {
        std::cout << GREEN "successful" WHITE " read command queue from a file \'" << fileName << "\';" << std::endl;
    }
    return outCommandsQueue;
}

/* private methods */
bool Reader::validatingReadedCommand(std::string const &command) const {
    bool isValidCurrentCommand = false;

    for (size_t j = ~0ULL; MAX_VALID_NO_PARAM_COMMANDS > ++j;) {
        if (command == _validCommandsNoParams[j]) {
            isValidCurrentCommand = true;
        }
    }

    for (size_t j = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++j;) {
        if (!command.compare(0, _validCommandsWithParams[j].length(), _validCommandsWithParams[j].c_str())) {
            isValidCurrentCommand
                = this->validatingCommandParam(command.substr(_validCommandsWithParams[j].length(),
                    _validCommandsWithParams[j].length() - command.length()));
        }
    }

    if (false == isValidCurrentCommand) {
        std::cout << ERR_PREFIX "\'";

        const size_t isSpaceInCommand = command.find_first_of(' ', 0);
        if (isSpaceInCommand < command.length()) {
            std::cout << command.substr(0, isSpaceInCommand);
        } else {
            std::cout << command;
        }
        std::cout  << "\' is an invalid command or missed\\invalid parameter;" << std::endl;
    }
    return isValidCurrentCommand;
}

bool Reader::validatingCommandParam(std::string const &param) const {
    bool isValid = true;
    size_t i = ~0ULL;
    std::string _param = std::string(param);
    if (' ' != param[0]) {
        std::cout << ERR_PREFIX "missed space \' \' after command with param;" << std::endl;
        return false;
    } else {
        _param = param.substr(1, param.length() - 1);
    }
    while (MaxOperandTypes > ++i) {
        if (!_param.compare(0, _validPushParamTypes[i].length(), _validPushParamTypes[i].c_str())) {
            const size_t _paramValueTypeParamStartBracketPos = _param.find_first_of('(', _validPushParamTypes[i].length());
            const size_t _paramValueTypeParamEndBracketPos = _param.find_first_of(')', _validPushParamTypes[i].length());
            std::string _paramValue;

            if (_paramValueTypeParamStartBracketPos < _param.length()) {
                _paramValue = _param.substr(_validPushParamTypes[i].length() + 1, _param.length() - _validPushParamTypes[i].length() - 2);
                if (_paramValue.empty()) {
                    std::cout << ERR_PREFIX "missed value for \'" << _validPushParamTypes[i] << "\';" <<  std::endl;
                    isValid = false;
                } else {
                    if ('-' == _paramValue[0]) {
                        _paramValue = _paramValue.substr(1, _paramValue.length() - 1);
                    }
                }
            } else {
                std::cout << ERR_PREFIX << "missed value starts bracket \'(\' for \'" << _validPushParamTypes[i] << "\';" << std::endl;
                isValid = false;
            }
            if (_paramValueTypeParamEndBracketPos < _param.length() && _paramValueTypeParamEndBracketPos + 1 == _param.length()) {
                if (3 > i) {
                    isValid = !_paramValue.empty()
                        && std::find_if(_paramValue.begin(), _paramValue.end(), [](char c) { return !std::isdigit(c); }) == _paramValue.end();
                    if (!isValid) {
                        std::cout << ERR_PREFIX << "value \'" << _paramValue <<  "\' in parameter for decimal type must to be only digits and decimal number;" << std::endl;
                    }
                } else {
                    bool isValidExponent = true, isValidMantissa = false;

                    const size_t floatDotInParam = _paramValue.find_first_of('.', 0);
                    const std::string exponent = (floatDotInParam < _paramValue.length())
                        ? _paramValue.substr(0, floatDotInParam) : std::string(_paramValue);
                    if (!exponent.empty()) {
                        isValidExponent = std::find_if(exponent.begin(), exponent.end(), [](char c) { return !std::isdigit(c); }) == exponent.end();
                        if (!isValidExponent) {
                            std::cout << ERR_PREFIX "exponent value \'" << exponent << "\' must to contain only digits and decimal number;" << std::endl;
                        }
                    }
                    if (floatDotInParam < _paramValue.length()) {
                        const std::string mantissa = _paramValue.substr(floatDotInParam + 1, _paramValue.length() - floatDotInParam - 1);
                        isValidMantissa = true;
                        if (mantissa.empty() && exponent.empty()) {
                            isValidMantissa = false;
                            std::cout << ERR_PREFIX << "invalid value, at least mantissa OR exponent must exist;" << std::endl;
                        } else {
                            isValidMantissa = std::find_if(mantissa.begin(), mantissa.end(), [](char c) { return !std::isdigit(c); }) == mantissa.end();
                            if (!isValidMantissa) {
                                std::cout << ERR_PREFIX "mantissa value \'" << mantissa << "\' must to contain only digits and decimal number;" << std::endl;
                            }
                        }
                    }
                    return isValidExponent && isValidMantissa;
                }
            } else {
                if (_paramValueTypeParamEndBracketPos < _param.length() && _paramValueTypeParamEndBracketPos + 1 != _param.length()) {
                    std::cout << ERR_PREFIX << "trash detected after ending bracket: \'" << _param.substr(_paramValueTypeParamEndBracketPos + 1, _param.length() - _paramValueTypeParamEndBracketPos) << "\'; " << std::endl;
                } else {
                    std::cout << ERR_PREFIX << "missed value ending bracket for \'" << _validPushParamTypes[i] << "\' \')\';" << std::endl;
                }
                isValid = false;
            }
            return isValid;
        }
    }
    std::cout << ERR_PREFIX "invalid type \'" << _param << "\' in command parameter;" << std::endl;
    return false;
}
