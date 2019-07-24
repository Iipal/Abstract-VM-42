#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <unistd.h>
#include <algorithm>
#include <stdio.h>
#include "IOperand.hpp"

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
    std::vector<std::string> *readMultiFileStackInput(int ac, char *av[]) const;

private:
    void printHelpInfoForStandardInput(void) const;

    bool specList(std::vector<std::string> *const commandQueue) const;
    bool specClean(std::vector<std::string> *const commandQueue) const;
    bool specDelete(std::vector<std::string> *const commandQueue) const;

    typedef bool (Reader::*fnptrInputSpecCommands)(std::vector<std::string> *const) const;
    const fnptrInputSpecCommands fnptrSpecFuncs[MAX_SPECIFIED_COMMANDS] = {
        &Reader::specList, &Reader::specClean, &Reader::specDelete };

};
