#include "processingCommandQueue.hpp"

bool processPrint(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'print\' can't display top value;" << std::endl;
        return false;
    } else {
        std::cout << "\'print\' top value: \'" << (*(o->begin()))->toString() << "\';" << std::endl;
    }
    return true;
}

bool processExit(std::list<IOperand const*> *const o) { (void)o; return true; }

bool processAdd(std::list<IOperand const*> *const o) {
    if  (2 > o->size()) {
        std::cout << WARN_PREFIX "can't process \'add\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' \'add\' \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand + *rightOperand;
        if (result) {
            std::cout << "\'" << (*result).toString() << "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'add\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool processSub(std::list<IOperand const*> *const o) {
    if (2 > o->size()) {
        std::cout << WARN_PREFIX "can't process \'sub\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' \'sub\' \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand - *rightOperand;
        if (result) {
            std::cout << "\'" << (*result).toString() << "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'sub\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool processMul(std::list<IOperand const*> *const o) {
    if (2 > o->size()) {
        std::cout << WARN_PREFIX "can't process \'mul\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' \'mul\' \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand * *rightOperand;
        if (result) {
            std::cout << "\'" << (*result).toString() << "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'mul\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool processDiv(std::list<IOperand const*> *const o) {
    if (2 > o->size()) {
        std::cout << WARN_PREFIX "can't process \'div\' command because at the top of the stack less then 2 values;" << std::endl;
        return false;
    } else {
        IOperand const *leftOperand = *(o->begin());
        IOperand const *rightOperand = *(++o->begin());
        std::cout << "\'" << (*leftOperand).toString() << "\' \'div\' \'" << (*rightOperand).toString() << "\' = ";
        IOperand const *result = *leftOperand / *rightOperand;
        if (result) {
            std::cout << "\'" << (*result).toString() << "\';" << std::endl;
            o->pop_front(); o->pop_front();
            o->push_front(result);
        } else {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "something went wrong when processing \'div\';" << std::endl;
            return false;
        }
    }
    return true;
}

bool processMod(std::list<IOperand const*> *const o) { (void)o; return true; }

bool processPop(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'pop\' can't unstack value from top;" << std::endl;
        return false;
    } else {
        std::cout << "pop " << (*(o->begin()))->toString() << " value from the top of the stack" << std::endl;
        o->pop_front();
    }
    return true;
}

bool processDump(std::list<IOperand const*> *const o) {
    if (!o->size()) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
            "command queue is empty now, \'dump\' can't print all stack values;" << std::endl;
        return false;
    } else {
        std::cout << "stack dump:" << std::endl;
        size_t elementNumber = ~0ULL;
        std::list<IOperand const*>::const_iterator it = o->begin();
        while (o->end() != it) {
            std::cout << std::setiosflags(std::ios::right) << "[" UNDERLINE << std::setw(6)
                << ++elementNumber << WHITE "] - " << (*it)->toString() << ";" << std::endl;
            ++it;
        }
    }
    return true;
}
