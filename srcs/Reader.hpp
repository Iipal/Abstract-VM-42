#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <unistd.h>

#define MAX_VALID_NO_PARAM_COMMANDS 9
#define MAX_VALID_W_PARAM_COMMANDS 2

#define RED "\033[31m"
#define WHITE "\033[0m"
#define INVERT "\033[7m"

#define ERR_PREFIX INVERT "AVM " WHITE RED "error" WHITE ": "

class Reader {
public:
    Reader();
    Reader(Reader const &copy);
    virtual ~Reader();

    Reader &operator=(Reader const &copy);

    std::vector<std::string> *readStandardInput(void) const;
    std::vector<std::string> *readFileInput(std::string const &fileName) const;
    bool validatingReadedCommandQueue(std::vector<std::string> *commandQueue) const;
};
