#pragma once

#include <regex>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include "Validation.hpp"

#define MAX_SPECIFIED_COMMANDS 4

class ReadStandardInputFeatures {
public:
    ReadStandardInputFeatures();
    ReadStandardInputFeatures(ReadStandardInputFeatures const &copy);
    virtual ~ReadStandardInputFeatures();

    ReadStandardInputFeatures &operator=(ReadStandardInputFeatures const &copy);


    std::string const &getHostName(void) const;
    std::string const &getFullExecutablePath(void) const;

    bool parseSpecCommands(std::string const &command, std::vector<std::string> &commandQueue) const;

private:
    std::string _hostName;
    std::string _fullExecutablePath;

    const std::string _specCommands[MAX_SPECIFIED_COMMANDS] = { "list", "clean", "delete", "help" };
    const std::string _shortSpecCommands[MAX_SPECIFIED_COMMANDS] = { "l", "c", "d", "h" };

    bool specList(std::vector<std::string> &commandQueue) const;
    bool specClean(std::vector<std::string> &commandQueue) const;
    bool specDelete(std::vector<std::string> &commandQueue) const;
    bool specHelp(void) const;

    typedef bool (ReadStandardInputFeatures::*fnptrSpecCommands)(std::vector<std::string>&) const;
    const fnptrSpecCommands fnptrSpecFuncs[MAX_SPECIFIED_COMMANDS] = {
        &ReadStandardInputFeatures::specList, &ReadStandardInputFeatures::specClean, &ReadStandardInputFeatures::specDelete };
};
