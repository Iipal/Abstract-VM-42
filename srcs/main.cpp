#include "Reader.hpp"
#include "Validation.hpp"
#include "OperandFactory.hpp"
#include "LaunchAVM.hpp"
#include <unistd.h>

std::vector<std::string> *mainCheckMultiFileStackInput(int &argc, char *argv[], Reader const &r);

int main(int argc, char *argv[]) {
    --argc; ++argv;

    Reader r;
    LaunchAVM l;
    std::vector<std::string> *commandQueue = NULL;

    if (!argc) {
        if (isatty(fileno(stdin))) {
            commandQueue = r.readStandardInput();
        } else { commandQueue = r.readPipeInput(); }
    } else {
        int const oldArgc = argc;
        commandQueue = mainCheckMultiFileStackInput(argc, argv, r);
        if (oldArgc != argc) { ++argv; }

        if (!commandQueue) {
            if (1 == argc) {
                commandQueue = r.readFileInput(*argv);
            } else {
                for (int i = 0; argc > i; i++) {
                    commandQueue = r.readFileInput(argv[i]);
                    if (commandQueue) {
                        l.launchAVM(commandQueue);
                        delete commandQueue;
                        commandQueue = NULL;
                    }
                }
            }
        }
    }

    if (commandQueue) {
        l.launchAVM(commandQueue);
        delete commandQueue;
    }
}

std::vector<std::string> *mainCheckMultiFileStackInput(int &argc, char *argv[], Reader const &r) {
    std::vector<std::string> *commandQueue = NULL;

    bool isMultiFileStackMode = false;
    if (std::string(*argv) == "-mfs") {
        isMultiFileStackMode = true;
        --argc; ++argv;
    }

    if (isMultiFileStackMode) {
        if (!argc) {
            std::cout << ERR_N_PREFIX(Validation::incrementGlobalErrorsCounter())
                ORANGE "multi file stack" WHITE " flag detected without any files input in arguments;" << std::endl;
        } else if (1 == argc) {
            std::cout << WARN_PREFIX ORANGE "multi file stack" WHITE " is useless for only 1 file;" << std::endl << std::endl;
        } else {
            std::cout << AVM_PREFIX ORANGE "multi file stack" WHITE " mode is activated:" << std::endl << std::endl;
            commandQueue = r.readMultiFileStackInput(argc, argv);
        }
    }

    return commandQueue;
}
