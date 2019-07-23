#include "Reader.hpp"
#include "OperandFactory.hpp"
#include "LaunchAVM.hpp"

void mainMultiFileStackInput(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    --argc; ++argv;

    if (!argc) {
        Reader r;
        std::vector<std::string> *commandQueue = NULL;

        if (isatty(fileno(stdin))) {
            commandQueue = r.readStandardInput();
        } else {
            commandQueue = r.readPipeInput();
        }
        if (commandQueue) {
            LaunchAVM l;
            l.launchAVM(commandQueue);
            delete commandQueue;
        }
    } else {
        mainMultiFilesInput(argc, argv);
    }
}

void mainMultiFileStackInput(int argc, char *argv[]) {
    Reader r;
    LaunchAVM l;
    std::vector<std::string> *commandQueue = NULL;

    bool isMultiFileStack = false;

    if (std::string(*argv) == "-mfs") {
        --argc; ++argv;

        if (1 == argc) {
            std::cout << WARN_PREFIX ORANGE "multi file stack" WHITE " is useless for only 1 file;" << std::endl << std::endl;
        } else if (!argc) {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
                ORANGE "multi file stack" WHITE " flag detected without any file;" << std::endl;
            return ;
        } else {
            std::cout << AVM_PREFIX ORANGE "multi file stack" WHITE " mode is activated:" << std::endl << std::endl;
            isMultiFileStack = true;
        }
    }

    for (int i = -1; argc > ++i;) {
        std::cout << AVM_PREFIX CYAN "file" WHITE " input mode( [" UNDERLINE
            << std::setw(3) << i + 1 << WHITE "]: " << argv[i] << " ):" << std::endl;

        commandQueue = r.readFileInput(argv[i], commandQueue);
        if (commandQueue) {
            if (!isMultiFileStack) {
                l.launchAVM(commandQueue);
                delete commandQueue;
                commandQueue = NULL;
            }

            if (isMultiFileStack && argc != i + 1) {
                if ((*commandQueue)[commandQueue->size() - 1] == "exit") {
                    commandQueue->pop_back();
                }
            }
        }

        if (argc != i + 1) { std::cout << std::endl; }
    }

    if (commandQueue && isMultiFileStack) {
        l.launchAVM(commandQueue);
        delete commandQueue;
    }
}
