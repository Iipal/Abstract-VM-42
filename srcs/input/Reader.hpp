#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include "IOperand.hpp"

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
};
