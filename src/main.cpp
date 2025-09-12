#include "cli.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        CLI cli(argc, argv);
        cli.run();
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
        return 1;
    }
    return 0;
}
