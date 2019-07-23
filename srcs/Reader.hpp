#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
#include <stdio.h>
#include "IOperand.hpp"

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
#define ERR_N_PREFIX(errNum) AVM_PREFIX RED "error" WHITE << " [" UNDERLINE << std::setw(6) << errNum << WHITE "]: "
#define ERR_REPORT_PREFIX AVM_PREFIX ORANGE "error-report" WHITE "⤵ : "
#define REPORT_PREFIX AVM_PREFIX BLUE "report" WHITE "      ⤴ : "
#define WARN_PREFIX AVM_PREFIX MAGENTA "warning" WHITE "     → : "

#define MAX_SPECIFIED_COMMANDS 3

class Reader {
public:
    Reader();
    Reader(Reader const &copy);
    virtual ~Reader();

    Reader &operator=(Reader const &copy);

    std::vector<std::string> *readStandardInput(void) const;
    std::vector<std::string> *readPipeInput(void) const;
    std::vector<std::string> *readFileInput(std::string const &fileName, std::vector<std::string> *commandQueue) const;

    static size_t const &incrementGlobalErrorsCounter(void) { return ++Reader::globalErrorsCounter; }
    static size_t const &getGlobalErrorsCounter(void) { return Reader::globalErrorsCounter; }
    static void refreshGlobalErrorsCounter(void) { Reader::globalErrorsCounter = 0; }

private:
    void printHelpInfoForStandardInput(void) const;

    void baseStringPrepareAfterReading(std::string &command) const;

    bool specList(std::vector<std::string> *const commandQueue) const;
    bool specClean(std::vector<std::string> *const commandQueue) const;
    bool specDelete(std::vector<std::string> *const commandQueue) const;

    typedef bool (Reader::*fnptrInputSpecCommands)(std::vector<std::string> *const) const;
    const fnptrInputSpecCommands fnptrSpecFuncs[MAX_SPECIFIED_COMMANDS] = {
        &Reader::specList, &Reader::specClean, &Reader::specDelete };

    bool validatingReadedCommand(std::string &command) const;

    bool validatingCommandParam(std::string &commandParam) const;
    bool validatingCommandParamValueInRange(std::string const &paramValue, eOperandType type,
        std::string &commandParam, bool const &isNegative,
        size_t const &paramValueTypeParamStartBracketPos) const;

    static size_t globalErrorsCounter;
};
