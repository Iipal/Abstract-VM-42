#include "ReadStandardInputFeatures.hpp"
#include "Reader.hpp"

/* global\static\const variables for class: */
class Validation v;

/* public methods */
Reader::Reader() { }
Reader::Reader(Reader const &copy) { *this = copy; }
Reader::~Reader() { }
Reader &Reader::operator=(Reader const &copy) { (void)copy; return *this; }

std::vector<std::string> *Reader::readStandardInput(void) const {
    ReadStandardInputFeatures f;
    Validation::refreshGlobalErrorsCounter();

    std::vector<std::string> *commandQueue = new std::vector<std::string>();
    if (!commandQueue) {
        std::cout << ERR_N_PREFIX "cannot allocate memory;" << std::endl;
        return commandQueue;
    }

    std::cout << AVM_PREFIX ORANGE "standard" WHITE " input mode (" UNDERLINE "'h' for details" WHITE "):" << std::endl;

    std::string tmp;
    bool isValidLastCommand = true;
    bool isValidInput = true;
    bool _exit = false;
    while (!_exit) {
        std::cout << BLUE "AVM" WHITE "@" INVERT << f.getHostName() << WHITE " "
            << (isValidLastCommand ? GREEN : RED) << "➜" WHITE " " << f.getFullExecutablePath() << WHITE ": ";
        isValidLastCommand = true;

        std::getline(std::cin, tmp);
        if (std::cin.bad() || std::cin.eof() || std::cin.fail()) {
            std::cout << ERR_N_PREFIX "Error occured in standard input;" << std::endl;
            _exit = true;
            isValidInput = false;
        } else {
            if (";;" != tmp) { v.baseStringPrepareAfterReading(tmp); }

            if (tmp.size()) {
                if (";;" == tmp) {
                    _exit = true;
                } else if ("q" == tmp || "quit" == tmp) {
                    _exit = true; isValidInput = false;
                } else {
                    if (!f.parseSpecCommands(tmp, *commandQueue)) {
                        if ((isValidLastCommand = v.validatingReadedCommand(tmp))) {
                            commandQueue->push_back(tmp);
                        } else { std::cout << WARN_PREFIX "invalid command detected, it was ignored. try another command. see help for more details ('h');" << std::endl; }
                    }
                }
            }
        }
    }

    if (isValidInput && !commandQueue->size()) {
        std::cout << std::endl << ERR_N_PREFIX "command queue is empty, can't execute AVM." << std::endl;
        isValidInput = false;
    }

    if (!isValidInput) { delete commandQueue; commandQueue = NULL; }

    return commandQueue;
}

std::vector<std::string> *Reader::readPipeInput(void) const {
    Validation::refreshGlobalErrorsCounter();

    std::vector<std::string> *commandQueue = new std::vector<std::string>();
    if (!commandQueue) {
        std::cout << ERR_N_PREFIX "cannot allocate memory;" << std::endl;
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
            std::cout << std::endl << ERR_N_PREFIX "error occured in pipe input;" << std::endl;
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
        std::cout << std::endl << ERR_N_PREFIX
            "invalid command queue. " UNDERLINE RED "can't execute AVM" WHITE ";" << std::endl;
        delete commandQueue; commandQueue = NULL;
    } else { std::cout << AVM_PREFIX BLUE "pipe" WHITE " " UNDERLINE "successful" WHITE " read;" << std::endl; }

    return commandQueue;
}

std::vector<std::string> *Reader::readFileInput(std::string const &fileName) const {
    Validation::refreshGlobalErrorsCounter();

    std::vector<std::string> *commandQueue = new std::vector<std::string>();
    if (!commandQueue) {
        std::cout << ERR_N_PREFIX "cannot allocate memory;" << std::endl;
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
            std::cout << std::endl << ERR_N_PREFIX
                "command queue is empty, can't execute AVM." << std::endl;
            isValid = false;
        }
    } else {
        std::cout << ERR_N_PREFIX "\'" << fileName << "\' file is invalid." << std::endl;
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
