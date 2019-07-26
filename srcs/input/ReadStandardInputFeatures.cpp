#include "ReadStandardInputFeatures.hpp"

ReadStandardInputFeatures::ReadStandardInputFeatures() {
    char hostNameBuff[64];
    gethostname(hostNameBuff, 64);
    _hostName = std::string(hostNameBuff);
    if (_hostName.empty()) { _hostName = std::string("AVM host-name"); }

    char *fullExecutablePathBuff = NULL;
    fullExecutablePathBuff = getcwd(fullExecutablePathBuff, 1024);
    if (fullExecutablePathBuff) {
        _fullExecutablePath = std::string(fullExecutablePathBuff);
        _fullExecutablePath.append("/avm");
        free(fullExecutablePathBuff);
    } else { _fullExecutablePath = std::string("~/error"); }
}
ReadStandardInputFeatures::ReadStandardInputFeatures(ReadStandardInputFeatures const &copy) { *this = copy; }
ReadStandardInputFeatures::~ReadStandardInputFeatures() { }

ReadStandardInputFeatures &ReadStandardInputFeatures::operator=(ReadStandardInputFeatures const &copy) {
    if (this != &copy) {
        _hostName = copy._hostName;
        _fullExecutablePath = copy._fullExecutablePath;
    }
    return *this;
}

std::string const &ReadStandardInputFeatures::getHostName(void) const { return _hostName; }
std::string const &ReadStandardInputFeatures::getFullExecutablePath(void) const { return _fullExecutablePath; }

bool ReadStandardInputFeatures::parseSpecCommands(std::string const &command, std::vector<std::string> &commandQueue) const {
    bool isValid = false;
    for (size_t i = 0; MAX_SPECIFIED_COMMANDS > i && !isValid; i++) {
        if (command == _shortSpecCommands[i] || command == _specCommands[i]) {
            if (3 == i) {
                isValid = specHelp();
            } else  { isValid = (this->*fnptrSpecFuncs[i])(commandQueue); }
        }
    }
    return isValid;
}

/* private methods */
bool ReadStandardInputFeatures::specList(std::vector<std::string> &commandQueue) const {
    if (!commandQueue.size()) {
        std::cout << WARN_PREFIX "can't show list of commands, queue is empty;" << std::endl;
    }

    size_t i = ~0ULL;
    while (commandQueue.size() > ++i) {
        std::cout << "    [" UNDERLINE << std::setw(6) << i + 1 << WHITE "]: " << commandQueue[i];

        if (!commandQueue[i].compare(commandQueue[i].find_first_of(' ', 0) + 1, 4, "int8")) {
            std::istringstream iss(commandQueue[i].substr(commandQueue[i].find_first_of('(', 0) + 1, commandQueue[i].length() - commandQueue[i].find_first_of('(', 0) - 2));
            int8_t value;
            iss >> value;
            if (0x20 <= value && 0x7f > value) {
                std::cout << DIM " : \'" << static_cast<char>(value) << "\'" WHITE;
            }
        }
        std::cout << ';' << std::endl;
    }
    return true;
}

bool ReadStandardInputFeatures::specClean(std::vector<std::string> &commandQueue) const {
    if (!commandQueue.size()) {
        std::cout << WARN_PREFIX "can't clean commands queue, it's empty;" << std::endl;
    }

    size_t i = commandQueue.size();
    while (i--) {
        std::cout << "    " RED "deleted" WHITE " : " << commandQueue[i] << std::endl;
        commandQueue.pop_back();
    }

    return true;
}

bool ReadStandardInputFeatures::specDelete(std::vector<std::string> &commandQueue) const {
    if (!commandQueue.size()) {
        std::cout << WARN_PREFIX "can't delete last command from queue, it's empty;" << std::endl;
    } else {
        std::cout << "    " RED "deleted" WHITE " : " << commandQueue[commandQueue.size() - 1] << std::endl;
        commandQueue.pop_back();
    }

    return true;
}

bool ReadStandardInputFeatures::specHelp(void) const {
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
        return true;
}
