#include "Reader.hpp"
#include "OperandFactory.hpp"

void processingCommandQueue(std::vector<std::string> *commandQueue);

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
        std::cout << "Usage: ./avm <file-name>.avm" << std::endl;
    }

    if (commandQueue) {
        processingCommandQueue(commandQueue);
    }
}
