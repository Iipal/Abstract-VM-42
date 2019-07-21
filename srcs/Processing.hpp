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
    bool processPush(std::string const &param, std::list<IOperand const*> *const o);
    bool processAssert(std::string const &param, std::list<IOperand const*> *const o);

    typedef bool (Processing::*fnptrProcessCommandWParam)(std::string const&, std::list<IOperand const*> *const);
    const fnptrProcessCommandWParam fnptrsCommandsWParam[MAX_VALID_W_PARAM_COMMANDS] = {
        &Processing::processPush, &Processing::processAssert };

    bool processPrint(std::list<IOperand const*> *const o);
    bool processExit(std::list<IOperand const*> *const o);

    bool baseProcessAriphmetic(std::list<IOperand const *> *const o, std::string const command, char const op);
    bool processAdd(std::list<IOperand const*> *const o);
    bool processSub(std::list<IOperand const*> *const o);
    bool processMul(std::list<IOperand const*> *const o);
    bool processDiv(std::list<IOperand const*> *const o);
    bool processMod(std::list<IOperand const*> *const o);

    bool processPop(std::list<IOperand const*> *const o);
    bool processDump(std::list<IOperand const*> *const o);

    typedef bool (Processing::*fnptrProcessCommandNoParam)(std::list<IOperand const*> *const);
    const fnptrProcessCommandNoParam fnptrsCommandsNoParam[MAX_VALID_NO_PARAM_COMMANDS] = {
        &Processing::processPrint, &Processing::processExit, &Processing::processAdd,
        &Processing::processSub, &Processing::processMul, &Processing::processDiv,
        &Processing::processMod, &Processing::processPop, &Processing::processDump };

};
