#include "processingCommandQueue.hpp"

bool processPrint(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, can't print top value;" << std::endl;
        return false;
    } else {
        std::cout << "print value from top \'" << *(o->begin()) << "\';" << std::endl;
    }
    return true;
}

bool processExit(std::list<IOperand const*> *const o) { (void)o; return true; }

bool processAdd(std::list<IOperand const*> *const o) { (void)o; return true; }
bool processSub(std::list<IOperand const*> *const o) { (void)o; return true; }
bool processMul(std::list<IOperand const*> *const o) { (void)o; return true; }
bool processDiv(std::list<IOperand const*> *const o) { (void)o; return true; }
bool processMod(std::list<IOperand const*> *const o) { (void)o; return true; }

bool processPop(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, can't unstack value from top;" << std::endl;
        return false;
    } else {
        o->pop_front();
    }
    return true;
}

bool processDump(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, can't print all values from the stack;" << std::endl;
        return false;
    } else {
        std::cout << "stack dump:" << std::endl;
        size_t elementNumber = ~0ULL;
        std::list<IOperand const*>::const_iterator it = o->begin();
        while (o->end() != it) {
            std::cout << "[" UNDERLINE << std::setw(6) << ++elementNumber << WHITE "] - \'" << (*it)->toString() << "\';" << std::endl;
            ++it;
        }
    }
    return true;
}
