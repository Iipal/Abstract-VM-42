#include "Reader.hpp"

int main(int argc, char *argv[]) {
    --argc; ++argv;

    Reader r;
    std::list<std::string> *commandQueue = NULL;

    if (!argc) {
        commandQueue = r.readStandardInput();
    } else if (1 == argc) {
        commandQueue = r.readFileInput(*argv);
    } else {
        std::cout << "Usage: ./avm <file-name>.avm" << std::endl;
    }

    if (commandQueue) {
        std::cout << "start AVM" << std::endl;
    }
}
