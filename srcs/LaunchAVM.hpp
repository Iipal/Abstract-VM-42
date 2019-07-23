#pragma once

#include "Reader.hpp"
#include "OperandFactory.hpp"

class LaunchAVM {
public:
    LaunchAVM();
    LaunchAVM(LaunchAVM const &copy);
    virtual ~LaunchAVM();

    LaunchAVM &operator=(const LaunchAVM &copy);

    bool launchAVM(std::vector<std::string> *commandQueue);

private:
    std::list<IOperand const *> *_operands;
    bool _isClear;
    bool const &isClear() const;
    void clear();

    bool parsePush(std::string const &param);
    bool parseAssert(std::string const &param);

    typedef bool (LaunchAVM::*fnptrParseCommandWParam)(std::string const&);
    const fnptrParseCommandWParam fnptrsCommandsWParam[MAX_VALID_W_PARAM_COMMANDS] = {
        &LaunchAVM::parsePush, &LaunchAVM::parseAssert };

    bool parseExit();
    bool parsePop();

    void baseDisplay(IOperand const *it, size_t i);
    bool parsePrint();
    bool parseDump();

    bool baseAriphmetic(std::string const command, char const op);
    bool parseAdd();
    bool parseSub();
    bool parseMul();
    bool parseDiv();
    bool parseMod();

    typedef bool (LaunchAVM::*fnptrParseCommandNoParam)(void);
    const fnptrParseCommandNoParam fnptrsCommandsNoParam[MAX_VALID_NO_PARAM_COMMANDS] = {
        &LaunchAVM::parseExit, &LaunchAVM::parsePop, &LaunchAVM::parsePrint, &LaunchAVM::parseDump,
        &LaunchAVM::parseAdd, &LaunchAVM::parseSub, &LaunchAVM::parseMul, &LaunchAVM::parseDiv,
        &LaunchAVM::parseMod, };

    void displayUnexecutedCommands(std::vector<std::string> *commandQueue, std::vector<std::string>::iterator &it, size_t const &commandsCounter);
};
