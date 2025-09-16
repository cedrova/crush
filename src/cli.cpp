#include "cli.hpp"
#include "compressor.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>

CLI::CLI(int argc, char* argv[]) {
    parse(argc, argv);
}

void CLI::run() const {
    Compressor comp;
    switch (mode) {
        case Mode::Compress:
            std::cout << "[crush] Compressing: " << inputFile
                      << " -> " << outputFile
                      << " (algo=" << algoToString(algo)
                      << ", level=" << compressionLevel
                      << ", threads=" << threads << ")\n";
            comp.compress(inputFile, outputFile, algo, compressionLevel, threads);
            break;
        case Mode::Decompress:
            std::cout << "[crush] Decompressing: " << inputFile
                      << " -> " << outputFile << "\n";
            comp.decompress(inputFile, outputFile);
            break;
        case Mode::Benchmark:
            std::cout << "[crush] Benchmark: " << inputFile
                      << " (algo=" << algoToString(algo) << ")\n";
            comp.benchmark(inputFile, algo);
            break;
        case Mode::Help:
            printHelp();
            break;
        default:
            throw std::invalid_argument("invalid command. use --help");
    }
}

void CLI::parse(int argc, char* argv[]) {
    if (argc < 2) { mode = Mode::Help; return; }

    std::vector<std::string> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string &a = args[i];
        if (a == "compress" || a == "-c") {
            if (i + 2 >= args.size()) throw std::invalid_argument("-c requires <input> <output>");
            mode = Mode::Compress;
            inputFile = args[++i];
            outputFile = args[++i];
        } else if (a == "decompress" || a == "-d") {
            if (i + 2 >= args.size()) throw std::invalid_argument("-d requires <archive> <output_dir>");
            mode = Mode::Decompress;
            inputFile = args[++i];
            outputFile = args[++i];
        } else if (a == "-b") {
            if (i + 1 >= args.size()) throw std::invalid_argument("-b requires <input>");
            mode = Mode::Benchmark;
            inputFile = args[++i];
        } else if (a == "-a" || a == "--algo") {
            if (i + 1 >= args.size()) throw std::invalid_argument("-a requires <algorithm>");
            algo = parseAlgo(args[++i]);
            if (algo == Algorithm::None) throw std::invalid_argument("unknown algorithm");
        } else if (a == "--level" || a == "-level") {
            if (i + 1 >= args.size()) throw std::invalid_argument("-level requires <1-9>");
            compressionLevel = std::stoi(args[++i]);
            if (compressionLevel < 1 || compressionLevel > 9) throw std::invalid_argument("level must be 1..9");
        } else if (a == "-p" || a == "--threads") {
            if (i + 1 >= args.size()) throw std::invalid_argument("-p requires <num>");
            threads = std::stoi(args[++i]);
            if (threads < 1) threads = 1;
        } else if (a == "--help" || a == "-h") {
            mode = Mode::Help;
        } else {
            std::ostringstream os;
            os << "unknown argument: " << a;
            throw std::invalid_argument(os.str());
        }
    }

    if (mode == Mode::Compress || mode == Mode::Benchmark) {
        if (algo == Algorithm::None) algo = Algorithm::Huffman;
    }
}

Algorithm CLI::parseAlgo(const std::string& arg) {
    if (arg == "huff" || arg == "huffman") return Algorithm::Huffman;
    if (arg == "lz77") return Algorithm::LZ77;
    if (arg == "bwt") return Algorithm::BWT;
    if (arg == "arith") return Algorithm::Arithmetic;
    return Algorithm::None;
}

std::string CLI::algoToString(Algorithm a) {
    switch (a) {
        case Algorithm::Huffman: return "Huffman";
        case Algorithm::LZ77: return "LZ77";
        case Algorithm::BWT: return "BWT";
        case Algorithm::Arithmetic: return "Arithmetic";
        default: return "None";
    }
}

void CLI::printHelp() const {
    std::cout <<
    "Crush - Phase3 CLI\n\n"
    "Usage:\n"
    "  crush compress <input> <output> [-a <algo>] [--level N] [-p threads]\n"
    "  crush decompress <archive> <output_dir>\n"
    "  crush -b <input> -a <algo>\n\n"
    "Algorithms: huff | lz77 | bwt | arith\n"
    "Examples:\n"
    "  crush compress file.txt file.crush -a huff --level 6 -p 4\n"
    "  crush decompress file.crush ./outdir\n";
}
