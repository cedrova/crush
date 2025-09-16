#include "compressor.hpp"  // must come before using Algorithm
#include "cli.hpp"
#include <iostream>

int CLI::run(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage:\n"
                  << "  crush -c <input> <output>\n"
                  << "  crush -d <input> <output>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string inFile = argv[2];
    std::string outFile = argv[3];

    if (mode == "-c") {
        Compressor::compress(inFile, outFile, Algorithm::Huffman);
    } else if (mode == "-d") {
        Compressor::decompress(inFile, outFile);
    } else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    return 0;
}
