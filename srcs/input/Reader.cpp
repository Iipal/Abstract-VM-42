#include "Reader.hpp"
#include "Validation.hpp"

/* global\static\const variables for class: */
class Validation v;

/* public methods */
Reader::Reader() { }
Reader::Reader(Reader const &copy) { *this = copy; }
Reader::~Reader() { }
Reader &Reader::operator=(Reader const &copy) { (void)copy; return *this; }

std::vector<std::string> *Reader::readStandardInput(void) const {
    Validation::refreshGlobalErrorsCounter();

    std::vector<std::string> *commandQueue = new std::vector<std::string>();
    if (!commandQueue) {
        std::cout << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter()) "cannot allocate memory;" << std::endl;
        return commandQueue;
    }

    std::string _hostName;
    std::string _fullExecutablePath;
    {
        char _hnBuff[32];
        gethostname(_hnBuff, 32);
        _hostName = std::string(_hnBuff);
        if (_hostName.empty()) {
            _hostName = std::string("*AVM host-name*");
        }

        char *_fepBuff = NULL;
        _fepBuff = getcwd(_fepBuff, 64);
        _fullExecutablePath = std::string(_fepBuff);
        _fullExecutablePath.append("/avm");
        free(_fepBuff);
    }

    std::cout << AVM_PREFIX ORANGE "standard" WHITE " input mode (" UNDERLINE "'h' for details" WHITE "):" << std::endl;

    const std::string _specCommands[MAX_SPECIFIED_COMMANDS] = { "list", "clean", "delete" };
    const std::string _shortSpecCommands[MAX_SPECIFIED_COMMANDS] = { "l", "c", "d" };

    std::string tmp;
    bool isValidLastCommand = true;
    bool isValidInput = true;
    bool _exit = false;
    while (!_exit) {
        std::cout << BLUE "AVM" WHITE "@" INVERT << _hostName << WHITE " "
            << (isValidLastCommand ? GREEN : RED) << "➜" WHITE " " << _fullExecutablePath << WHITE ": ";
        isValidLastCommand = true;

        std::getline(std::cin, tmp);
        if (std::cin.bad() || std::cin.eof() || std::cin.fail()) {
            std::cout << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter()) "Error occured in standard input;" << std::endl;
            _exit = true;
            isValidInput = false;
        } else {
            if (";;" != tmp) {
                v.baseStringPrepareAfterReading(tmp);
            }
            if (tmp.size()) {
                if (";;" == tmp) {
                    _exit = true;
                } else if ("q" == tmp || "quit" == tmp) {
                    _exit = true; isValidInput = false;
                } else if ("h" == tmp || "help" == tmp) {
                    printHelpInfoForStandardInput();
                } else {
                    bool isSpec = false;

                    size_t i = ~0ULL;
                    while (MAX_SPECIFIED_COMMANDS > ++i && !isSpec) {
                        if (tmp == _shortSpecCommands[i] || tmp == _specCommands[i]) {
                            isSpec = (this->*fnptrSpecFuncs[i])(commandQueue);
                        }
                    }

                    if (!isSpec) {
                        if ((isValidLastCommand = v.validatingReadedCommand(tmp))) {
                            commandQueue->push_back(tmp);
                        } else {
                            std::cout << WARN_PREFIX "invalid command detected, it was ignored."
                                " try another command. see help for more details ('h');" << std::endl;
                        }
                    }
                }
            }
        }
    }

    if (isValidInput && !commandQueue->size()) {
        std::cout << std::endl << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter())
                "command queue is empty, can't execute AVM." << std::endl;
        isValidInput = false;
    }
    if (!isValidInput) {
        delete commandQueue;
        commandQueue = NULL;
    }
    return commandQueue;
}

std::vector<std::string> *Reader::readPipeInput(void) const {
    Validation::refreshGlobalErrorsCounter();

    std::vector<std::string> *commandQueue = new std::vector<std::string>();
    if (!commandQueue) {
        std::cout << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter()) "cannot allocate memory;" << std::endl;
        return commandQueue;
    }

    std::cout << AVM_PREFIX BLUE "pipe" WHITE " input mode:" << std::endl;

    std::string tmp;
    bool isValid = true;
    bool quit = false;
    while (!quit) {
        if (!std::getline(std::cin, tmp)) {
            quit = true;
        } else if (std::cin.bad() || std::cin.fail()) {
            std::cout << std::endl << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter()) "error occured in pipe input;" << std::endl;
            quit = true; isValid = false;
        } else {
            v.baseStringPrepareAfterReading(tmp);
            if (tmp.length()) {
                if (v.validatingReadedCommand(tmp)) {
                    commandQueue->push_back(tmp);
                } else { isValid = false; }
            }
        }
    }

    if (!isValid) {
        std::cout << std::endl << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter())
            "invalid command queue. " UNDERLINE RED "can't execute AVM" WHITE ";" << std::endl;
        delete commandQueue; commandQueue = NULL;
    } else { std::cout << AVM_PREFIX BLUE "pipe" WHITE " " UNDERLINE "successful" WHITE " read;" << std::endl; }

    return commandQueue;
}

std::vector<std::string> *Reader::readFileInput(std::string const &fileName) const {
    Validation::refreshGlobalErrorsCounter();

    std::vector<std::string> *commandQueue = new std::vector<std::string>();
    if (!commandQueue) {
        std::cout << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter()) "cannot allocate memory;" << std::endl;
        return commandQueue;
    }

    bool isValid = true;
    std::fstream file(fileName);
    if (file.is_open()) {
        std::string tmp;
        size_t readedLines = 0;

        while (std::getline(file, tmp)) {
            ++readedLines;
            v.baseStringPrepareAfterReading(tmp);
            if (tmp.length()) {
                if (v.validatingReadedCommand(tmp)) {
                    commandQueue->push_back(tmp);
                } else {
                    std::cout << REPORT_PREFIX "error on line: [" UNDERLINE << std::setw(5) << readedLines << WHITE "];" << std::endl;
                    isValid = false;
                }
            }
        }

        if (isValid && !commandQueue->size()) {
            std::cout << std::endl << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter())
                "command queue is empty, can't execute AVM." << std::endl;
            isValid = false;
        }
    } else {
        std::cout << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter()) "\'" << fileName << "\' file is invalid." << std::endl;
        isValid = false;
    }

    if (!isValid) {
        std::cout << std::endl << WARN_PREFIX "at least [" RED UNDERLINE << std::setw(6) << Validation::getGlobalErrorsCounter()
            << WHITE "] error occured before AVM was executed," " try to fix all error reports above for successful AVM work;" << std::endl;
        delete commandQueue; commandQueue = NULL;
    } else { std::cout << AVM_PREFIX CYAN "file" WHITE " \'" << fileName << "\' " UNDERLINE "successful" WHITE " read;" << std::endl; }

    return commandQueue;
}

std::vector<std::string> *Reader::readMultiFileStackInput(int ac, char *av[]) const {
    std::vector<std::string> *commandQueue = NULL;
    std::vector<std::string> *tempQueue = NULL;

    for (int i = 0; ac > i; i++) {
        std::cout << AVM_PREFIX CYAN "file" WHITE " input mode( [" UNDERLINE << std::setw(3) << i + 1 << WHITE "]: " << av[i] << " ):" << std::endl;
        tempQueue = readFileInput(av[i]);
        if (tempQueue) {
            if (i + 1 != ac) {
                if ((*tempQueue)[tempQueue->size() - 1] == "exit") {
                    tempQueue->pop_back();
                }
            }

            if (!commandQueue) { commandQueue = new std::vector<std::string>(); }

            for (std::vector<std::string>::iterator it = tempQueue->begin(); tempQueue->end() != it; it++) {
                commandQueue->push_back(*it);
            }

            delete tempQueue; tempQueue = NULL;
        } else {
            std::cout << REPORT_PREFIX "invalid file \'" << av[i] << "\' ignored. fix this file and try again;" << std::endl;
        }

        if (i + 1 != ac) { std::cout << std::endl; }
    }

    return commandQueue;
}

/* private methods */
void Reader::printHelpInfoForStandardInput(void) const {
    std::cout << std::endl << std::setiosflags(std::ios::left)
        << "| "        " command "       " | " << std::setw(14) << "parameter"     << std::setw(95) << ": description" << '|' << std::endl
        << "| " INVERT "exit     " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stop to execute command queue and exit from AVM (necessary at the end of command queue);" << '|' << std::endl
        << "| " INVERT "print    " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Asserts that the value at the top of the stack is an 8-bit integer;" << '|' << std::endl
        << "| " INVERT "assert   " WHITE " | " << std::setw(14) << "@exception"    << std::setw(95) << ": check if @exception is true or not;" << '|' << std::endl
        << "| " INVERT "push     " WHITE " | " << std::setw(14) << "@type(@value)" << std::setw(95) << ": valid @type is int8, int16, int32, float, double; Pushes the @value at the top of the stack;" << '|' << std::endl
        << "| " INVERT "pop      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the value from the top of the stack;" << '|' << std::endl
        << "| " INVERT "add      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, adds them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "sub      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, subtracts them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "mul      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, multiplies them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "div      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, divides them, then stacks the result;" << '|' << std::endl
        << "| " INVERT "mod      " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Unstacks the first two values on the stack, calculates the modulus, then stacks the result;" << '|' << std::endl
        << "| " INVERT ";        " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": All comments start with ';';" << '|' << std::endl
        << "| " INVERT "         " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Empty lines will be ignored;" << '|' << std::endl
        << "Specified, for standard input mode, commands:" << std::endl
        << "| " INVERT ";;       " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stops waiting for any input and execute AVM;" << '|' << std::endl
        << "| " INVERT "clean/c  " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Remove all commands from queue;" << '|' << std::endl
        << "| " INVERT "delete/d " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Remove last added command from queue;" << '|' << std::endl
        << "| " INVERT "quit/q   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Stops waiting for any input and quit without execute AVM;" << '|' << std::endl
        << "| " INVERT "list/l   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Show list of currently added commands to queue;" << '|' << std::endl
        << "| " INVERT "help/h   " WHITE " | " << std::setw(14) << ' '             << std::setw(95) << ": Print this help info;" << '|' << std::endl << std::endl;
}

bool Reader::specList(std::vector<std::string> *const commandQueue) const {
    if (!commandQueue->size()) {
        std::cout << WARN_PREFIX "can't show list of commands, queue is empty;" << std::endl;
    }

    size_t i = ~0ULL;
    while (commandQueue->size() > ++i) {
        std::cout << "    [" UNDERLINE << std::setw(6) << i + 1 << WHITE "]: " << (*commandQueue)[i] << std::endl;
    }
    return true;
}

bool Reader::specClean(std::vector<std::string> *const commandQueue) const {
    if (!commandQueue->size()) {
        std::cout << WARN_PREFIX "can't clean commands queue, it's empty;" << std::endl;
    }

    size_t i = commandQueue->size();
    while (i--) {
        std::cout << "    " RED "deleted" WHITE " : " << (*commandQueue)[i] << std::endl;
        commandQueue->pop_back();
    }

    return true;
}

bool Reader::specDelete(std::vector<std::string> *const commandQueue) const {
    if (!commandQueue->size()) {
        std::cout << WARN_PREFIX "can't delete last command from queue, it's empty;" << std::endl;
    } else {
        std::cout << "    " RED "deleted" WHITE " : " << (*commandQueue)[commandQueue->size() - 1] << std::endl;
        commandQueue->pop_back();
    }

    return true;
}
