#pragma once
#include <string>
#include "compressor.hpp"

enum class Mode { Compress, Decompress, Benchmark, Help, Invalid };

class CLI {
public:
    CLI(int argc, char* argv[]);
    void run() const;

private:
    Mode mode{Mode::Invalid};
    Algorithm algo{Algorithm::None};
    std::string inputFile;
    std::string outputFile;
    int compressionLevel{5};
    int threads{1};

    void parse(int argc, char* argv[]);
    static Algorithm parseAlgo(const std::string& arg);
    static std::string algoToString(Algorithm a);
    void printHelp() const;
};
