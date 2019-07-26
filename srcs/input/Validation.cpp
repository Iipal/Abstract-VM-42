#include "Validation.hpp"

size_t Validation::_globalErrorsCounter = 0;

Validation::Validation() { }
Validation::Validation(Validation const &copy) { *this = copy; }
Validation::~Validation() { }
Validation &Validation::operator=(Validation const &copy) { (void)copy; return *this; }

size_t const &Validation::incrementGlobalErrorsCounter(void) { return ++Validation::_globalErrorsCounter; }
size_t const &Validation::getGlobalErrorsCounter(void) { return Validation::_globalErrorsCounter; }
void Validation::refreshGlobalErrorsCounter(void) { Validation::_globalErrorsCounter = 0; }

void Validation::baseStringPrepareAfterReading(std::string &command) const {
    if (command.length()) {
        const size_t isCommentaryExistAfterCommand = command.find_first_of(';', 0);
        if (isCommentaryExistAfterCommand < command.length()) {
            command = command.substr(0, isCommentaryExistAfterCommand);
        }
        command.erase(std::find_if(command.rbegin(), command.rend(), [](int ch) { return !std::isspace(ch); }).base(), command.end());
    }
}

bool Validation::validatingReadedCommand(std::string &command) const {
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

            isValidCurrentCommand = validatingCommandParam(commandParam);
            if (isValidCurrentCommand) {
                command.erase(_validCommandsWithParams[i].length() + 1);
                command.append(commandParam);
            }
        }
    }

    if (!isValidCurrentCommand) {
        std::cout << ERR_N_PREFIX "\'" INVERT;

        const size_t isSpaceInCommand = command.find_first_of(' ', 0);
        if (isSpaceInCommand < command.length()) {
            std::cout << command.substr(0, isSpaceInCommand);
        } else { std::cout << command; }

        std::cout  << WHITE "\' command is invalid ('" CYAN << command << WHITE "');" << std::endl;
        Validation::incrementGlobalErrorsCounter();
    }
    return isValidCurrentCommand;
}

static inline bool validatingCommandParamValueIsDigits(std::string const &paramValue) {
    bool out = !paramValue.empty()
        && std::find_if(paramValue.begin(), paramValue.end(), [](char c) { return !std::isdigit(c); }) == paramValue.end();
    if (!out) {
        std::cout << ERR_REPORT_PREFIX "value \'" INVERT << paramValue
            << WHITE "\' must to contain only digits and decimal number;" << std::endl;
    }
    return out;
}

bool Validation::validatingCommandParam(std::string &commandParam) const {
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

                    std::string exponent = _paramValue;
                    const size_t floatDotInParam = _paramValue.find_first_of('.', 0);
                    if (floatDotInParam < _paramValue.length()) {
                        exponent.erase(floatDotInParam + 1);
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
                    if (isValidExponent && isValidMantissa) {
                        return validatingCommandParamValueInRange(_paramValue, static_cast<eOperandType>(i), commandParam, isNegative, _paramValueTypeParamStartBracketPos);
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

template <typename T>
static inline bool baseValidatingCommandParamValueInRange(std::string const &paramValue, T &trueValue, eOperandType const &type, bool isNegative) {
    bool isValid = true;

    try {
        int64_t newIntValue = 0;
        long double newFloatValue = 0.0;

        if (Float > type) {
            newIntValue = std::stol(paramValue) * (isNegative ? -1 : 1);
        } else {
            newFloatValue = std::stold(paramValue) * (isNegative ? -1 : 1);
        }

        switch (type) {
            case Int8: {
                if (INT8_MAX < newIntValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newIntValue
                        << WHITE "\' is overflow of max type int8 value: " UNDERLINE << INT8_MAX << WHITE ";" << std::endl;
                    isValid = false;
                } else if (INT8_MIN > newIntValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newIntValue
                        << WHITE "\' is underflow of min type int8 value: " UNDERLINE << INT8_MIN << WHITE ";" << std::endl;
                    isValid = false;
                } else {
                    trueValue = newIntValue;
                }
                break;
            }
            case Int16: {
                if (INT16_MAX < newIntValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newIntValue
                        << WHITE "\' is overflow of max type int16 value: " UNDERLINE << INT16_MAX << WHITE ";" << std::endl;
                    isValid = false;
                } else if (INT16_MIN > newIntValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newIntValue
                        << WHITE "\' is underflow of min type int16 value: " UNDERLINE << INT16_MIN << WHITE ";" << std::endl;
                    isValid = false;
                } else {
                    trueValue = newIntValue;
                }
                break;
            }
            case Int32: {
                if (INT32_MAX < newIntValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newIntValue
                        << WHITE "\' is overflow of max type int32 value: " UNDERLINE << INT32_MAX << WHITE ";" << std::endl;
                    isValid = false;
                } else if (INT32_MIN > newIntValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newIntValue
                        << WHITE "\' is underflow of min type int32 value: " UNDERLINE << INT32_MIN << WHITE ";" << std::endl;
                    isValid = false;
                } else {
                    trueValue = newIntValue;
                }
                break;
            }
            case Float: {
                if (__FLT_MAX__ < newFloatValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newFloatValue
                        << WHITE "\' is overflow of max type float value: " UNDERLINE << __FLT_MAX__ << WHITE ";" << std::endl;
                    isValid = false;
                } else if (__FLT_MIN__ > newFloatValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newFloatValue
                        << WHITE "\' is underflow of min type float value: " UNDERLINE << __FLT_MIN__ << WHITE ";" << std::endl;
                    isValid = false;
                } else {
                    trueValue = newFloatValue;
                }
                break;
            }
            case Double: {
                if (__DBL_MAX__ < newFloatValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newFloatValue
                        << WHITE "\' is overflow of max type double value: " UNDERLINE << __DBL_MAX__ << WHITE ";" << std::endl;
                    isValid = false;
                } else if (__DBL_MIN__ > newFloatValue) {
                    std::cout << ERR_REPORT_PREFIX "\'" RED UNDERLINE << newFloatValue
                        << WHITE "\' is underflow of min type double value: " UNDERLINE << __DBL_MIN__ << WHITE ";" << std::endl;
                    isValid = false;
                } else {
                    trueValue = newFloatValue;
                }
                break;
            }
            default: break;
        }
    } catch (std::exception &e) {
        std::cout << ERR_REPORT_PREFIX << "invalid value for reading with \'std::" << e.what()
            << "\', min: " UNDERLINE << std::numeric_limits<int64_t>::min()
            << WHITE ", max: " << std::numeric_limits<int64_t>::max() << WHITE ", but underflow\\overflow value for current type ";
        switch (type) {
            case Int8:   std::cout << "int8 is: " UNDERLINE   << static_cast<int32_t>(std::numeric_limits<int8_t>::min())  << WHITE " - " UNDERLINE << static_cast<int32_t>(std::numeric_limits<int8_t>::max())  << WHITE ";" << std::endl; break;
            case Int16:  std::cout << "int16 is: " UNDERLINE  << std::numeric_limits<int16_t>::min() << WHITE " - " UNDERLINE << std::numeric_limits<int16_t>::max() << WHITE ";" << std::endl; break;
            case Int32:  std::cout << "int32 is: " UNDERLINE  << std::numeric_limits<int32_t>::min() << WHITE " - " UNDERLINE << std::numeric_limits<int32_t>::max() << WHITE ";" << std::endl; break;
            case Float:  std::cout << "float is: " UNDERLINE  << std::numeric_limits<float>::min()   << WHITE " - " UNDERLINE << std::numeric_limits<float>::max()   << WHITE ";" << std::endl; break;
            case Double: std::cout << "double is: " UNDERLINE << std::numeric_limits<double>::min()  << WHITE " - " UNDERLINE << std::numeric_limits<double>::max()  << WHITE ";" << std::endl; break;
            default: break;
        }
        isValid = false;
    }
    return isValid;
}

template <typename T>
static inline void baseConvertCommandParamInRangeValue(T const resValue, std::string &commandParam, size_t const &bracket) {
    commandParam.erase(bracket + 1);
    commandParam.append(std::to_string(resValue) + ")");
}

bool Validation::validatingCommandParamValueInRange(std::string const &paramValue, eOperandType type,
    std::string &commandParam, bool isNegative, size_t bracket) const {
    bool isValid = true;

    long double trueFloatValue = 0.0;
    int64_t trueIntValue = 0;
    if (Float > type) {
        isValid = baseValidatingCommandParamValueInRange(paramValue, trueIntValue, type, isNegative);
    } else {
        isValid = baseValidatingCommandParamValueInRange(paramValue, trueFloatValue, type, isNegative);
    }

    if (isValid) {
        trueIntValue *= (isNegative ? -1 : 1);
        trueFloatValue *= (isNegative ? -1.0 : 1.0);
        switch (type) {
            case Int8: baseConvertCommandParamInRangeValue(static_cast<int8_t>(trueIntValue), commandParam, bracket); break;
            case Int16: baseConvertCommandParamInRangeValue(static_cast<int16_t>(trueIntValue), commandParam, bracket); break;
            case Int32: baseConvertCommandParamInRangeValue(static_cast<int32_t>(trueIntValue), commandParam, bracket); break;

            case Float: baseConvertCommandParamInRangeValue(static_cast<float>(trueFloatValue), commandParam, bracket); break;
            case Double: baseConvertCommandParamInRangeValue(static_cast<double>(trueFloatValue), commandParam, bracket); break;

            default: break;
        }
    }

    return isValid;
}
