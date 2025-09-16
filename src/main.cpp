#include "cli.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        // Construct CLI with command-line arguments
        CLI cli(argc, argv);

        // Run the CLI (calls compress/decompress/benchmark as needed)
        cli.run();
    } catch (const std::exception& e) {
        std::cerr << "[crush] error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
