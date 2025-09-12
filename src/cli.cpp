#include "cli.hpp"
#include <iostream>
#include <stdexcept>

CLI::CLI(int argc, char* argv[]) {
    parse(argc, argv);
}

void CLI::run() const {
    switch (mode) {
        case Mode::Compress:
            std::cout << "[Stub] Compressing " << inputFile 
                      << " -> " << outputFile 
                      << " using " << algoToString(algo) << "\n";
            break;
        case Mode::Decompress:
            std::cout << "[Stub] Decompressing " << inputFile 
                      << " -> " << outputFile << "\n";
            break;
        case Mode::Benchmark:
            std::cout << "[Stub] Benchmarking " << inputFile 
                      << " with " << algoToString(algo) << "\n";
            break;
        case Mode::Help:
            printHelp();
            break;
        default:
            throw std::invalid_argument("Invalid command. Use --help for usage.");
    }
}

void CLI::parse(int argc, char* argv[]) {
    if (argc < 2) {
        mode = Mode::Help;
        return;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    mode = Mode::Invalid;
    algo = Algorithm::None;

    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "-c") {
            mode = Mode::Compress;
            if (i + 2 >= args.size())
                throw std::invalid_argument("Missing input/output for -c");
            inputFile = args[i + 1];
            outputFile = args[i + 2];
            i += 2;
        } else if (args[i] == "-d") {
            mode = Mode::Decompress;
            if (i + 2 >= args.size())
                throw std::invalid_argument("Missing input/output for -d");
            inputFile = args[i + 1];
            outputFile = args[i + 2];
            i += 2;
        } else if (args[i] == "-b") {
            mode = Mode::Benchmark;
            if (i + 1 >= args.size())
                throw std::invalid_argument("Missing input for -b");
            inputFile = args[i + 1];
            i += 1;
        } else if (args[i] == "-a") {
            if (i + 1 >= args.size())
                throw std::invalid_argument("Missing algorithm after -a");
            algo = parseAlgo(args[i + 1]);
            i += 1;
        } else if (args[i] == "--help") {
            mode = Mode::Help;
        }
    }
}

Mode CLI::parseMode(const std::string& arg) {
    if (arg == "-c") return Mode::Compress;
    if (arg == "-d") return Mode::Decompress;
    if (arg == "-b") return Mode::Benchmark;
    if (arg == "--help") return Mode::Help;
    return Mode::Invalid;
}

Algorithm CLI::parseAlgo(const std::string& arg) {
    if (arg == "huff") return Algorithm::Huffman;
    if (arg == "lz77") return Algorithm::LZ77;
    if (arg == "bwt") return Algorithm::BWT;
    if (arg == "arith") return Algorithm::Arithmetic;
    return Algorithm::None;
}

std::string CLI::algoToString(Algorithm algo) {
    switch (algo) {
        case Algorithm::Huffman: return "Huffman";
        case Algorithm::LZ77: return "LZ77";
        case Algorithm::BWT: return "Burrows–Wheeler";
        case Algorithm::Arithmetic: return "Arithmetic";
        default: return "None";
    }
}

void CLI::printHelp() const {
    std::cout << "Crush - Compression and Archiving Tool\n";
    std::cout << "Usage:\n";
    std::cout << "  crush -c <input> <output> -a <algorithm>\n";
    std::cout << "  crush -d <archive> <output_dir>\n";
    std::cout << "  crush -b <input> -a <algorithm>\n\n";
    std::cout << "Algorithms: huff | lz77 | bwt | arith\n";
}
