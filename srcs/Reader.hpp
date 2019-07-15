#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
#include <stdio.h>
#include "IOperand.hpp"

#define MAX_VALID_NO_PARAM_COMMANDS 9
#define MAX_VALID_W_PARAM_COMMANDS 2

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

class Reader {
public:
    Reader();
    Reader(Reader const &copy);
    virtual ~Reader();

    Reader &operator=(Reader const &copy);

    std::vector<std::string> *readStandardInput(void) const;
    std::vector<std::string> *readPipeInput(void) const;
    std::vector<std::string> *readFileInput(std::string const &fileName) const;

private:
    bool validatingReadedCommand(std::string const &command) const;
    bool validatingCommandParam(std::string const &_pushType) const;
    void printHelpInfoForStandardInput(void) const;

    static const std::string _validCommandsNoParams[MAX_VALID_NO_PARAM_COMMANDS];
    static const std::string _validCommandsWithParams[MAX_VALID_W_PARAM_COMMANDS];

    static size_t globalErrorsCounter;
};
