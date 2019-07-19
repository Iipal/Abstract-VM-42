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

#define RED "\033[31m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define WHITE "\033[0m"
#define GREEN "\033[32m"
#define MAGENTA "\033[35m"

#define INVERT "\033[7m"
#define UNDERLINE "\033[4m"

#define ERR_N_PREFIX(errNum) CYAN "AVM" WHITE " " RED "error" WHITE << " [" UNDERLINE << std::setw(6) << errNum << WHITE "]: "
#define ERR_REPORT_PREFIX CYAN "AVM" WHITE " " RED "error-report" WHITE "⤵ : "
#define REPORT_PREFIX CYAN "AVM" WHITE " " CYAN "report" WHITE "      ⤴ : "
#define WARN_PREFIX CYAN "AVM" WHITE " " MAGENTA "warning" WHITE "     → : "

#define MAX_SPECIFIED_COMMANDS 3

class Reader {
public:
    Reader();
    Reader(Reader const &copy);
    virtual ~Reader();

    Reader &operator=(Reader const &copy);

    std::vector<std::string> *readStandardInput(void) const;
    std::vector<std::string> *readPipeInput(void) const;
    std::vector<std::string> *readFileInput(std::string const &fileName) const;

    static size_t incrementGlobalErrorsCounter(void) { return ++Reader::globalErrorsCounter; }
    static size_t const &getGlobalErrorsCounter(void) { return Reader::globalErrorsCounter; }

private:
    bool validatingReadedCommand(std::string &command) const;
    bool validatingCommandParam(std::string &commandParam) const;

    void printHelpInfoForStandardInput(void) const;

    bool specList(std::vector<std::string> *const commandQueue) const;
    bool specClean(std::vector<std::string> *const commandQueue) const;
    bool specDelete(std::vector<std::string> *const commandQueue) const;

    typedef bool (Reader::*fnptrInputSpecCommands)(std::vector<std::string> *const) const;
    const fnptrInputSpecCommands fnptrSpecFuncs[MAX_SPECIFIED_COMMANDS] = {
        &Reader::specList, &Reader::specClean, &Reader::specDelete };

    static size_t globalErrorsCounter;
};
