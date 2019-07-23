#include "Reader.hpp"
#include "OperandFactory.hpp"
#include "Processing.hpp"

int main(int argc, char *argv[]) {
    --argc; ++argv;

    Reader r;
    Processing p;
    std::vector<std::string> *commandQueue = NULL;

    if (!argc) {
        if (isatty(fileno(stdin))) {
            commandQueue = r.readStandardInput();
        } else {
            commandQueue = r.readPipeInput();
        }
    } else {
        for (int i = -1; argc > ++i;) {
            std::cout << "    AVM " CYAN "file" WHITE " input mode( [" UNDERLINE
                << std::setw(3) << i + 1 << WHITE "]: " << argv[i] << " ):" << std::endl;

            commandQueue = r.readFileInput(argv[i]);
            if (commandQueue) {
                p.startProcessing(commandQueue);
                delete commandQueue; commandQueue = NULL;
            }

            if (i + 1 != argc) {
                std::cout << std::endl;
            }
        }
    }

    if (commandQueue) {
        p.startProcessing(commandQueue);
        delete commandQueue; commandQueue = NULL;
    }
}
