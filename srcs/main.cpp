#include "Reader.hpp"

int main(int argc, char *argv[]) {
    --argc; ++argv;

    Reader r;
    std::vector<std::string> *commandQueue = NULL;

    if (!argc) {
        commandQueue = r.readStandardInput();
    } else if (1 == argc) {
        commandQueue = r.readFileInput(*argv);
    }

    if (commandQueue) {
        if (r.validatingReadedCommandQueue(commandQueue)) {
            std::cout << "start AVM" << std::endl;
        }
    }
}
