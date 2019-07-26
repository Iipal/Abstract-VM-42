#pragma once

#include "IOperand.hpp"
#include "Validation.hpp"
#include <algorithm>
#include <iomanip>
#include <string>

#define RED "\033[38;5;1m"
#define BLUE "\033[38;5;75m"
#define CYAN "\033[38;5;87m"
#define WHITE "\033[0m"
#define GREEN "\033[38;5;112m"
#define ORANGE "\033[38;5;208m"
#define MAGENTA "\033[38;5;169m"

#define DIM "\033[2m"
#define INVERT "\033[7m"
#define UNDERLINE "\033[4m"

#define AVM_PREFIX "    AVM "
#define ERR RED "error" WHITE
#define ERR_N_PREFIX AVM_PREFIX ERR << " [" UNDERLINE << std::setw(6) << Validation::incrementGlobalErrorsCounter() << WHITE "]: "
#define ERR_REPORT_PREFIX AVM_PREFIX ORANGE "error-report" WHITE "⤵ : "
#define REPORT_PREFIX AVM_PREFIX BLUE "report" WHITE "      ⤴ : "
#define WARN_PREFIX AVM_PREFIX MAGENTA "warning" WHITE "     → : "

class Validation {
public:
    Validation();
    Validation(Validation const &copy);
    virtual ~Validation();

    Validation &operator=(Validation const &copy);

    static size_t const &incrementGlobalErrorsCounter(void);
    static size_t const &getGlobalErrorsCounter(void);
    static void refreshGlobalErrorsCounter(void);

    void baseStringPrepareAfterReading(std::string &command) const;

    bool validatingReadedCommand(std::string &command) const;
    bool validatingCommandParam(std::string &commandParam) const;
    bool validatingCommandParamValueInRange(std::string const &paramValue, eOperandType type,
        std::string &commandParam, bool isNegative, size_t bracket) const;

private:
    static size_t _globalErrorsCounter;
};
