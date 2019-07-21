#include "Reader.hpp"
#include "OperandFactory.hpp"
#include "Processing.hpp"

int main(int argc, char *argv[]) {
    --argc; ++argv;

    Reader r;
    std::vector<std::string> *commandQueue = NULL;

    if (!argc) {
        if (isatty(fileno(stdin))) {
            commandQueue = r.readStandardInput();
        } else {
            commandQueue = r.readPipeInput();
        }
    } else if (1 == argc) {
        commandQueue = r.readFileInput(*argv);
    } else {
        std::cout << "Usage:" << std::endl
            << " " CYAN "file" WHITE " input:     $> ./avm <file-name>.avm" << std::endl
            << " " BLUE "pipe" WHITE " input:     $> cat <file-name>.avm | ./avm" << std::endl
            << " " ORANGE "standard" WHITE " input: $> ./avm" << std::endl;
    }

    if (commandQueue) {
        Processing p;
        p.startProcessing(commandQueue);
        delete commandQueue;
    }
}
