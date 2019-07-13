#pragma once

#include <iostream>
#include <fstream>
#include <list>
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

#define ERR_PREFIX INVERT BLUE "AVM" WHITE " " RED "error" WHITE " " UNDERLINE "↓" WHITE " : "
#define ERR_N_PREFIX(errNum) INVERT BLUE "AVM" WHITE " " RED "error" WHITE "[" UNDERLINE << errNum << WHITE "]: "
#define WARN_PREFIX INVERT BLUE "AVM" WHITE " " MAGENTA "warning" WHITE ": "

class Reader {
public:
    Reader();
    Reader(Reader const &copy);
    virtual ~Reader();

    Reader &operator=(Reader const &copy);

    std::list<std::string> *readStandardInput(void) const;
    std::list<std::string> *readPipeInput(void) const;
    std::list<std::string> *readFileInput(std::string const &fileName) const;

private:
    bool validatingReadedCommand(std::string const &command) const;
    bool validatingCommandParam(std::string const &_pushType) const;

    static const std::string _validCommandsNoParams[MAX_VALID_NO_PARAM_COMMANDS];
    static const std::string _validCommandsWithParams[MAX_VALID_W_PARAM_COMMANDS];

    static const std::string _validPushParamTypes[MaxOperandTypes];
    static size_t globalErrorsCounter;
};
