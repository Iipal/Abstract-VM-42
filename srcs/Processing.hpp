#pragma once

#include "Reader.hpp"
#include "OperandFactory.hpp"

class Processing {
public:
    Processing();
    Processing(Processing const &copy);
    virtual ~Processing();

    Processing &operator=(const Processing &copy);

    bool startProcessing(std::vector<std::string> *commandQueue);
private:
    std::list<IOperand const *> *_operands;
    bool _isClear;
    bool const &isClear() const;
    void clear();

    bool processPush(std::string const &param);
    bool processAssert(std::string const &param);

    typedef bool (Processing::*fnptrProcessCommandWParam)(std::string const&);
    const fnptrProcessCommandWParam fnptrsCommandsWParam[MAX_VALID_W_PARAM_COMMANDS] = {
        &Processing::processPush, &Processing::processAssert };

    bool processExit();
    bool processPop();

    void baseDisplayOperand(IOperand const *it, size_t i);
    bool processPrint();
    bool processDump();

    bool baseProcessAriphmetic(std::string const command, char const op);
    bool processAdd();
    bool processSub();
    bool processMul();
    bool processDiv();
    bool processMod();

    typedef bool (Processing::*fnptrProcessCommandNoParam)(void);
    const fnptrProcessCommandNoParam fnptrsCommandsNoParam[MAX_VALID_NO_PARAM_COMMANDS] = {
        &Processing::processExit, &Processing::processPop, &Processing::processPrint, &Processing::processDump,
        &Processing::processAdd, &Processing::processSub, &Processing::processMul, &Processing::processDiv,
        &Processing::processMod, };
    void displayUnexecutedCommands(std::vector<std::string> *commandQueue, std::vector<std::string>::iterator &it, size_t const &commandsCounter);
};
