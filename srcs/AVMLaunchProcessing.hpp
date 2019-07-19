#pragma once

#include "Reader.hpp"
#include "OperandFactory.hpp"

class AVMLaunchProcessing {
public:
    AVMLaunchProcessing();
    AVMLaunchProcessing(AVMLaunchProcessing const &copy);
    virtual ~AVMLaunchProcessing();

    AVMLaunchProcessing &operator=(const AVMLaunchProcessing &copy);

    bool startProcessing(std::vector<std::string> *commandQueue);
private:
    bool processPush(std::string const &param, std::list<IOperand const*> *const o);
    bool processAssert(std::string const &param, std::list<IOperand const*> *const o);

    typedef bool (AVMLaunchProcessing::*fnptrProcessCommandWParam)(std::string const&, std::list<IOperand const*> *const);
    const fnptrProcessCommandWParam fnptrsCommandsWParam[MAX_VALID_W_PARAM_COMMANDS] = {
        &AVMLaunchProcessing::processPush, &AVMLaunchProcessing::processAssert };

    bool processPrint(std::list<IOperand const*> *const o);
    bool processExit(std::list<IOperand const*> *const o);
    bool processAdd(std::list<IOperand const*> *const o);
    bool processSub(std::list<IOperand const*> *const o);
    bool processMul(std::list<IOperand const*> *const o);
    bool processDiv(std::list<IOperand const*> *const o);
    bool processMod(std::list<IOperand const*> *const o);
    bool processPop(std::list<IOperand const*> *const o);
    bool processDump(std::list<IOperand const*> *const o);

    typedef bool (AVMLaunchProcessing::*fnptrProcessCommandNoParam)(std::list<IOperand const*> *const);
    const fnptrProcessCommandNoParam fnptrsCommandsNoParam[MAX_VALID_NO_PARAM_COMMANDS] = {
        &AVMLaunchProcessing::processPrint, &AVMLaunchProcessing::processExit, &AVMLaunchProcessing::processAdd,
        &AVMLaunchProcessing::processSub, &AVMLaunchProcessing::processMul, &AVMLaunchProcessing::processDiv,
        &AVMLaunchProcessing::processMod, &AVMLaunchProcessing::processPop, &AVMLaunchProcessing::processDump };

};
