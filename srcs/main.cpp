#include "Reader.hpp"
#include "OperandFactory.hpp"
#include "Processing.hpp"

void mainMultiFilesInput(int argc, char *argv[]);

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
            Processing p;
            p.startProcessing(commandQueue);
            delete commandQueue;
        }
    } else {
        mainMultiFilesInput(argc, argv);
    }
}

void mainMultiFilesInput(int argc, char *argv[]) {
    Reader r;
    Processing p;
    std::vector<std::string> *commandQueue = NULL;

    bool isMultiFileStack = false;

    if (std::string(*argv) == "-mfs") {
        bool isValid = true;
        if (1 == argc - 1) {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
                ORANGE "multi file stack" WHITE " is useless for only 1 file;" << std::endl; isValid = false;
        } else if (0 == argc - 1) {
            std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter())
                ORANGE "multi file stack" WHITE " flag detected without any file;" << std::endl; isValid = false;
        } else {
            std::cout << "    AVM " ORANGE "multi file stack" WHITE " mode is activated:" << std::endl << std::endl;
            isMultiFileStack = true; --argc; ++argv;
        }
        if (!isValid) { return ; }
    }
    if (!argc) {
        std::cout << ERR_N_PREFIX(Reader::incrementGlobalErrorsCounter()) "missed files in arguments;" << std::endl;
        return ;
    }

    for (int i = -1; argc > ++i;) {
        std::cout << "    AVM " CYAN "file" WHITE " input mode( [" UNDERLINE
            << std::setw(3) << i + 1 << WHITE "]: " << argv[i] << " ):" << std::endl;

        commandQueue = r.readFileInput(argv[i], commandQueue);
        if (commandQueue && !isMultiFileStack) {
            p.startProcessing(commandQueue);
            delete commandQueue;
            commandQueue = NULL;
        }
        if (isMultiFileStack && argc != i + 1) {
            if ((*commandQueue)[commandQueue->size() - 1] == "exit") {
                commandQueue->pop_back();
            }
        }

        if (argc != i + 1) {
            std::cout << std::endl;
        }
    }

    if (commandQueue && isMultiFileStack) {
        p.startProcessing(commandQueue);
        delete commandQueue;
    }
}
