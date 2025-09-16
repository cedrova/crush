#include "cli.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
    CLI cli;
    cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "[crush] error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
