#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char *argv[]) {
    --argc; ++argv;
    bool    start_work = false;

    std::vector<std::string> _commandQueue;
    if (!argc) {
        std::string _tmp;
        while (std::getline(std::cin, _tmp)) {
            if (_tmp != ";;") {
                _commandQueue.push_back(_tmp);
            } else {
                break ;
            }
        }
        start_work = true;
    } else if (argc == 1) {
        std::fstream _file(*argv);
        if (_file.is_open()) {
            std::string _tmp;
            while (std::getline(_file, _tmp)) {
                if (_tmp.compare(0, 1, ";")) {
                    _commandQueue.push_back(_tmp);
                }
            }
            start_work = true;
        } else {
            std::cout << "Cann't open " << *argv << "." << std::endl;
        }
    }
    if (start_work) {
        size_t i = ~0ULL;
        while (_commandQueue.size() > ++i) {
            std::cout << _commandQueue[i] << std::endl;
        }
    }
}
