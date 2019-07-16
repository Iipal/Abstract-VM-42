#pragma once

#include "../Reader.hpp"
#include "../OperandFactory.hpp"

typedef bool (*fnptrProcessCommandWParam)(std::string const&, std::list<IOperand const*> *const);
bool processPush(std::string const &param, std::list<IOperand const*> *const o);
bool processAssert(std::string const &param, std::list<IOperand const*> *const o);
const fnptrProcessCommandWParam fnptrsCommandsWParam[MAX_VALID_W_PARAM_COMMANDS] = { processPush, processAssert };

typedef bool (*fnptrProcessCommandNoParam)(std::list<IOperand const*> *const);
bool processPrint(std::list<IOperand const*> *const o);
bool processExit(std::list<IOperand const*> *const o);
bool processAdd(std::list<IOperand const*> *const o);
bool processSub(std::list<IOperand const*> *const o);
bool processMul(std::list<IOperand const*> *const o);
bool processDiv(std::list<IOperand const*> *const o);
bool processMod(std::list<IOperand const*> *const o);
bool processPop(std::list<IOperand const*> *const o);
bool processDump(std::list<IOperand const*> *const o);
const fnptrProcessCommandNoParam fnptrsCommandsNoParam[MAX_VALID_NO_PARAM_COMMANDS] = { processPrint, processExit,
                            processAdd, processSub, processMul, processDiv, processMod, processPop, processDump };
