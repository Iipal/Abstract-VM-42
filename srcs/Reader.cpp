#include "Reader.hpp"

/* global\static\const variables for class: */
size_t Reader::globalErrorsCounter = 0;

/* public methods */
Reader::Reader() { }
Reader::Reader(Reader const &copy) { *this = copy; }
Reader::~Reader() { }

Reader &Reader::operator=(Reader const &copy) {
    if (this != &copy) { *this = copy; }
    return *this;
}

std::vector<std::string> *Reader::readStandardInput(void) const {
    Reader::refreshGlobalErrorsCounter();

    std::vector<std::string> *outCommandsQueue = new std::vector<std::string>();
    if (!outCommandsQueue) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "cannot allocate memory;" << std::endl;
        return outCommandsQueue;
    }

    std::string _hostName;
    std::string _fullExecutablePath;
    {
        char _hnBuff[32];
        gethostname(_hnBuff, 32);
        _hostName = std::string(_hnBuff);
        if (_hostName.empty()) {
            _hostName = std::string("*AVM host-name*");
        }

        char *_fepBuff = NULL;
        _fepBuff = getcwd(_fepBuff, 64);
        _fullExecutablePath = std::string(_fepBuff);
        _fullExecutablePath.append("/avm");
        free(_fepBuff);
    }

    std::cout << "    AVM " ORANGE "standard" WHITE " input mode (" UNDERLINE "'h' for details" WHITE "):" << std::endl;

    const std::string _specCommands[MAX_SPECIFIED_COMMANDS] = { "list", "clean", "delete" };
    const std::string _shortSpecCommands[MAX_SPECIFIED_COMMANDS] = { "l", "c", "d" };

    std::string _tmp;
    bool isValidLastCommand = true;
    bool isValidInput = true;
    bool _exit = false;
    while (!_exit) {
        std::cout << BLUE "AVM" WHITE "@" INVERT << _hostName << WHITE " "
            << (isValidLastCommand ? GREEN : RED) << "➜" WHITE " " << _fullExecutablePath << WHITE ": ";
        isValidLastCommand = true;

        std::getline(std::cin, _tmp);
        if (std::cin.bad() || std::cin.eof() || std::cin.fail()) {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "Error occured in standard input;" << std::endl;
            _exit = true;
            isValidInput = false;
        } else {
            if (";;" != _tmp) {
                baseStringPrepareAfterReading(_tmp);
            }
            if (_tmp.size()) {
                if (";;" == _tmp) {
                    _exit = true;
                } else if ("q" == _tmp || "quit" == _tmp) {
                    _exit = true; isValidInput = false;
                } else if ("h" == _tmp || "help" == _tmp) {
                    printHelpInfoForStandardInput();
                } else {
                    bool isSpec = false;

                    size_t i = ~0ULL;
                    while (MAX_SPECIFIED_COMMANDS > ++i && !isSpec) {
                        if (_tmp == _shortSpecCommands[i] || _tmp == _specCommands[i]) {
                            isSpec = (this->*fnptrSpecFuncs[i])(outCommandsQueue);
                        }
                    }

                    if (!isSpec) {
                        if ((isValidLastCommand = validatingReadedCommand(_tmp))) {
                            outCommandsQueue->push_back(_tmp);
                        } else {
                            std::cout << WARN_PREFIX "invalid command detected, it was ignored."
                                " try another command. see help for more details ('h');" << std::endl;
                        }
                    }
                }
            }
        }
    }

    if (isValidInput && !outCommandsQueue->size()) {
        std::cout << WARN_PREFIX "command queue is empty, can't execute AVM;" << std::endl;
        isValidInput = false;
    }
    if (!isValidInput) {
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    }
    return outCommandsQueue;
}

std::vector<std::string> *Reader::readPipeInput(void) const {
    Reader::refreshGlobalErrorsCounter();

    std::vector<std::string> *outCommandsQueue = new std::vector<std::string>();
    if (!outCommandsQueue) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "cannot allocate memory;" << std::endl;
        return outCommandsQueue;
    }

    std::cout << "    AVM " BLUE "pipe" WHITE " input mode:" << std::endl;

    std::string _tmp;
    bool isValid = true;
    bool _exit = false;
    while (!_exit) {
        if (!std::getline(std::cin, _tmp)) {
            _exit = true;
        } else if (std::cin.bad() || std::cin.fail()) {
            std::cout << std::endl << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "error occured in pipe input;" << std::endl;
            _exit = true;
            isValid = false;
        } else if (_tmp.length()) {
            baseStringPrepareAfterReading(_tmp);
            if (_tmp.length()) {
                if (validatingReadedCommand(_tmp)) {
                    outCommandsQueue->push_back(_tmp);
                } else {
                    isValid = false;
                }
            }
        }
    }

    if (false == isValid) {
        std::cout << std::endl << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "invalid command queue. " UNDERLINE RED "can't execute AVM" WHITE ";" << std::endl;
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    } else {
        std::cout << " " UNDERLINE "successful" WHITE " read command queue from a pipe;" << std::endl;
    }
    return outCommandsQueue;
}

std::vector<std::string> *Reader::readFileInput(std::string const &fileName) const {
    Reader::refreshGlobalErrorsCounter();

    std::vector<std::string> *outCommandsQueue = new std::vector<std::string>();
    if (!outCommandsQueue) {
        std::cout << ERR_REPORT_PREFIX "cannot allocate memory;" << std::endl;
        return outCommandsQueue;
    }

    bool isValid = true;
    std::fstream _file(fileName);
    if (_file.is_open()) {
        size_t readedLines = 0;
        std::string _tmp;
        while (std::getline(_file, _tmp)) {
            ++readedLines;
            if (_tmp.length()) {
                baseStringPrepareAfterReading(_tmp);
                if (_tmp.length()) {
                    if (validatingReadedCommand(_tmp)) {
                        outCommandsQueue->push_back(_tmp);
                    } else {
                        std::cout << REPORT_PREFIX "error on line: [ " UNDERLINE
                            << std::setw(5) << readedLines << WHITE " ];" << std::endl;
                        isValid = false;
                    }
                }
            }
        }
        if (isValid && !outCommandsQueue->size()) {
            std::cout << std::endl << ERR_REPORT_PREFIX "command queue is empty, can't execute AVM." << std::endl;
            isValid = false;
        }
    } else {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "\'" << fileName << "\' file is invalid." << std::endl;
        isValid = false;
    }

    if (false == isValid) {
        std::cout << std::endl << CYAN "AVM" WHITE " " MAGENTA "work-report" WHITE "   : at least [" RED UNDERLINE
            << std::setw(6) << Reader::getGlobalErrorsCounter()
            << WHITE "] error occured before AVM was executed,"
            " try to fix all error reports above for successful AVM work;" << std::endl;
        delete outCommandsQueue;
        outCommandsQueue = NULL;
    } else {
        std::cout << UNDERLINE "successful" WHITE " read command queue from a file \'" << fileName << "\';" << std::endl;
    }
    return outCommandsQueue;
}

/* private methods */
void Reader::printHelpInfoForStandardInput(void) const {
    std::cout << std::endl << std::setiosflags(std::ios::left)
        << "| "        " command "       " | " << std::setw(14) << "parameter"     << std::setw(95) << ": description" << '|' << std::endl
        << "| " INVERT "exit     " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stop to execute command queue and exit from AVM (necessary at the end of command queue);" << '|' << std::endl
        << "| " INVERT "print    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Asserts that the value at the top of the stack is an 8-bit integer;" << '|' << std::endl
        << "| " INVERT "assert   " WHITE " | " << std::setw(14) << "@exception"    << std::setw(95) << ": check if @exception is true or not;" << '|' << std::endl
        << "| " INVERT "push     " WHITE " | " << std::setw(14) << "@type(@value)" << std::setw(95) << ": valid @type is int8, int16, int32, float, double; Pushes the @value at the top of the stack;" << '|' << std::endl
        << "| " INVERT "pop      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the value from the top of the stack;" << '|' << std::endl
        << "| " INVERT "add      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, adds them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "sub      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, subtracts them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "mul      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, multiplies them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "div      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, divides them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "mod      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, calculates the modulus, then stacks the result;" << '|' << std::endl
        << "| " INVERT "         " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Empty lines will be ignored;" << '|' << std::endl
        << "Specified, for standard input mode, commands:" << std::endl
        << "| " INVERT ";;       " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stops waiting for any input and execute AVM;" << '|' << std::endl
        << "| " INVERT "clean/c  " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Remove all commands from queue;" << '|' << std::endl
        << "| " INVERT "delete/d " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Remove last added command from queue;" << '|' << std::endl
        << "| " INVERT "quit/q   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stops waiting for any input and quit without execute AVM;" << '|' << std::endl
        << "| " INVERT "list/l   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Show list of currently added commands to queue;" << '|' << std::endl
        << "| " INVERT "help/h   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Print this help info;" << '|' << std::endl << std::endl;
}

void Reader::baseStringPrepareAfterReading(std::string &command) const {
    const size_t isCommentaryExistAfterCommand = command.find_first_of(';', 0);
    if (isCommentaryExistAfterCommand < command.length()) {
        command = command.substr(0, isCommentaryExistAfterCommand);
    }
    command.erase(std::find_if(command.rbegin(), command.rend(), [](int ch) { return !std::isspace(ch); }).base(), command.end());
}

bool Reader::specList(std::vector<std::string> *const commandQueue) const {
    if (!commandQueue->size()) {
        std::cout << WARN_PREFIX "can't show list of commands, queue is empty;" << std::endl;
    }

    size_t i = ~0ULL;
    while (commandQueue->size() > ++i) {
        std::cout << "    [" UNDERLINE << std::setw(6) << i + 1 << WHITE "]: " << (*commandQueue)[i] << std::endl;
    }
    return true;
}

bool Reader::specClean(std::vector<std::string> *const commandQueue) const {
    if (!commandQueue->size()) {
        std::cout << WARN_PREFIX "can't clean commands queue, it's empty;" << std::endl;
    }

    size_t i = commandQueue->size();
    while (i--) {
        std::cout << RED "deleted" WHITE ": " << (*commandQueue)[i] << std::endl;
        commandQueue->pop_back();
    }

    return true;
}

bool Reader::specDelete(std::vector<std::string> *const commandQueue) const {
    if (!commandQueue->size()) {
        std::cout << WARN_PREFIX "can't delete last command from queue, it's empty;" << std::endl;
    } else {
        std::cout << RED "deleted" WHITE ": " << (*commandQueue)[commandQueue->size() - 1] << std::endl;
        commandQueue->pop_back();
    }

    return true;
}

bool Reader::validatingReadedCommand(std::string &command) const {
    const size_t previousErrorsCounterState = Reader::globalErrorsCounter;
    bool isValidCurrentCommand = false;

    for (size_t i = ~0ULL; MAX_VALID_NO_PARAM_COMMANDS > ++i;) {
        if (command == _validCommandsNoParams[i]) {
            isValidCurrentCommand = true;
        }
    }

    for (size_t i = ~0ULL; MAX_VALID_W_PARAM_COMMANDS > ++i;) {
        if (!command.compare(0, _validCommandsWithParams[i].length(), _validCommandsWithParams[i].c_str())) {
            std::string commandParam = command.substr(_validCommandsWithParams[i].length(),
                    _validCommandsWithParams[i].length() - command.length());
            isValidCurrentCommand
                = this->validatingCommandParam(commandParam);
            if (isValidCurrentCommand) {
                std::string _srcCopy = command.substr(0, _validCommandsWithParams[i].length() + 1);
                _srcCopy.append(commandParam);
                command = std::string(_srcCopy);
            }
        }
    }

    if (false == isValidCurrentCommand) {
        std::cout << ERR_N_PREFIX(previousErrorsCounterState + 1) "\'" INVERT;

        const size_t isSpaceInCommand = command.find_first_of(' ', 0);
        if (isSpaceInCommand < command.length()) {
            std::cout << command.substr(0, isSpaceInCommand);
        } else {
            std::cout << command;
        }
        std::cout  << WHITE "\' command is invalid ('" CYAN << command << WHITE "');" << std::endl;
        Reader::incrementGlobalErrorsCounter();
    }
    return isValidCurrentCommand;
}

bool validatingCommandParamValueIsDigits(std::string const &paramValue) {
    bool out = !paramValue.empty()
        && std::find_if(paramValue.begin(), paramValue.end(), [](char c) { return !std::isdigit(c); }) == paramValue.end();
    if (!out) {
        std::cout << ERR_REPORT_PREFIX "value \'" INVERT << paramValue
            << WHITE "\' must to contain only digits and decimal number;" << std::endl;
    }
    return out;
}

bool Reader::validatingCommandParam(std::string &commandParam) const {
    if (' ' != commandParam[0]) {
        std::cout << ERR_REPORT_PREFIX "missed space \'" INVERT " " WHITE "\' after command which must to contain parameter;" << std::endl;
        return false;
    } else {
        commandParam = commandParam.substr(1, commandParam.length() - 1);
    }
    {
        const size_t additionalErrorSpaceInParamter = commandParam.find_first_of(' ', 0);
        if (additionalErrorSpaceInParamter < commandParam.length()) {
            std::cout << ERR_REPORT_PREFIX "detected invalid space in command parameter \'" INVERT
                << commandParam.substr(additionalErrorSpaceInParamter - 1, commandParam.length() - additionalErrorSpaceInParamter) << WHITE "\';" << std::endl;
            return false;
        }
    }

    bool isValid = true;
    bool isNegative = false;
    size_t i = ~0ULL;
    while (MaxOperandTypes > ++i) {
        if (!commandParam.compare(0, _validPushParamTypes[i].length(), _validPushParamTypes[i].c_str())) {
            const size_t _paramValueTypeParamStartBracketPos = commandParam.find_first_of('(', _validPushParamTypes[i].length());
            const size_t _paramValueTypeParamEndBracketPos = commandParam.find_first_of(')', _validPushParamTypes[i].length());
            std::string _paramValue;

            if (_paramValueTypeParamStartBracketPos < commandParam.length()) {
                _paramValue = commandParam.substr(_validPushParamTypes[i].length() + 1, commandParam.length() - _validPushParamTypes[i].length() - 2);
                if (_paramValue.empty()) {
                    std::cout << ERR_REPORT_PREFIX "missed value for \'" INVERT << _validPushParamTypes[i] << WHITE "\';" <<  std::endl;
                    isValid = false;
                } else if ('-' == _paramValue[0]) {
                    _paramValue = _paramValue.substr(1, _paramValue.length() - 1);
                    isNegative = true;
                }
            } else {
                std::cout << ERR_REPORT_PREFIX << "missed starts bracket \'" INVERT "(" WHITE "\' for \'" INVERT
                    << _validPushParamTypes[i] << WHITE "\';" << std::endl;
                isValid = false;
            }
            if (_paramValueTypeParamEndBracketPos < commandParam.length() && _paramValueTypeParamEndBracketPos + 1 == commandParam.length()) {
                if (3 > i) {
                    isValid = validatingCommandParamValueIsDigits(_paramValue);
                } else {
                    bool isValidExponent = true, isValidMantissa = true;

                    const size_t floatDotInParam = _paramValue.find_first_of('.', 0);
                    std::string exponent = _paramValue;
                    if (floatDotInParam < _paramValue.length()) {
                        exponent = _paramValue.substr(0, floatDotInParam);
                    }
                    isValidExponent = validatingCommandParamValueIsDigits(exponent);

                    if (floatDotInParam < _paramValue.length()) {
                        const std::string mantissa = _paramValue.substr(floatDotInParam + 1, _paramValue.length() - floatDotInParam - 1);
                        isValidMantissa = true;
                        if (mantissa.empty() && exponent.empty()) {
                            isValidMantissa = false;
                            std::cout << ERR_REPORT_PREFIX << "invalid value, at least mantissa OR exponent must exist;" << std::endl;
                        } else {
                            isValidMantissa = validatingCommandParamValueIsDigits(mantissa);
                        }
                    }
                    return isValidExponent && isValidMantissa;
                }
            } else {
                if (_paramValueTypeParamEndBracketPos < commandParam.length() && _paramValueTypeParamEndBracketPos + 1 != commandParam.length()) {
                    std::cout << ERR_REPORT_PREFIX << "trash detected after ending bracket: \'" INVERT
                        << commandParam.substr(_paramValueTypeParamEndBracketPos + 1, commandParam.length() - _paramValueTypeParamEndBracketPos)
                        << WHITE "\';" << std::endl;
                } else {
                    std::cout << ERR_REPORT_PREFIX << "missed ending bracket for \'" INVERT << _validPushParamTypes[i]
                        << WHITE "\' \'" INVERT ")" WHITE "\';" << std::endl;
                }
                isValid = false;
            }
            if (isValid) {
                isValid = validatingCommandParamValueInRange(_paramValue, static_cast<eOperandType>(i), commandParam, isNegative, _paramValueTypeParamStartBracketPos);
            }
            return isValid;
        }
    }
    const size_t _paramStartBrakcet = commandParam.find_first_of('(', 0);
    std::cout << ERR_REPORT_PREFIX "invalid type \'" INVERT;
    if (_paramStartBrakcet < commandParam.length()) {
        std::cout << commandParam.substr(0, _paramStartBrakcet);
    } else {
        std::cout << commandParam;
    }
    std::cout << WHITE "\';" << std::endl;
    return false;
}

bool baseIntValidatingCommandParamValueInRange(std::string const &paramValue, int32_t &_trueValue, eOperandType const &type) {
    bool out = true;
    try {
        _trueValue = std::stoi(paramValue);
    } catch (std::exception &e) {
        std::cout << ERR_REPORT_PREFIX << "invalid value for reading with \'std::" << e.what()
            << "\', max: " UNDERLINE << INT32_MAX << WHITE ", min: " UNDERLINE << INT32_MIN
            << WHITE;
            switch (type) {
                case Int8: std::cout << ", but with overflow to current type int8_t(" UNDERLINE
                    << INT8_MAX << WHITE " - " UNDERLINE << INT8_MIN << WHITE ");" << std::endl; break;
                case Int16: std::cout << ", but with overflow to current type int16_t(" UNDERLINE
                    << INT16_MAX << WHITE " - " UNDERLINE << INT16_MIN << WHITE ");" << std::endl; break;
                default: std::cout << ';' << std::endl; break;
            }
        out = false;
    }
    return out;
}

template <typename T>
void baseConvertCommandParamInRangeValue(T const resValue, std::string &commandParam, size_t const &bracket) {
    commandParam.erase(bracket + 1);
    commandParam.append(std::to_string(resValue) + ")");
}

bool Reader::validatingCommandParamValueInRange(std::string const &paramValue, eOperandType type,
    std::string &commandParam, bool const &isNegative, size_t const &bracket) const {
    bool isValid = true;
    int32_t _trueIntValue;
    double _trueFloatValue = std::stod(paramValue);
    if (Float > type) {
        isValid = baseIntValidatingCommandParamValueInRange(paramValue, _trueIntValue, type);
    }
    if (isValid) {
        switch (type) {
            case Int8: baseConvertCommandParamInRangeValue(static_cast<int8_t>(_trueIntValue) * (isNegative ? -1 : 1), commandParam, bracket); break;
            case Int16: baseConvertCommandParamInRangeValue(static_cast<int16_t>(_trueIntValue) * (isNegative ? -1 : 1), commandParam, bracket); break;
            case Int32: baseConvertCommandParamInRangeValue(_trueIntValue * (isNegative ? -1 : 1), commandParam, bracket); break;
            case Float: baseConvertCommandParamInRangeValue(static_cast<float>(_trueFloatValue) * (isNegative ? -1.0f : 1.0f), commandParam, bracket); break;
            case Double: baseConvertCommandParamInRangeValue(_trueFloatValue * (isNegative ? -1.0 : 1.0), commandParam, bracket); break;
            default: break;
        }
    }
    return isValid;
}
