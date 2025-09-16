#pragma once
#include <string>

// Reuse the CLI Algorithm enum if needed; compressor currently implements Huffman only.
enum class CompressorAlgo { Huffman = 0 };

struct Compressor {
    static void compress(const std::string& inFile, const std::string& outFile);
    static void decompress(const std::string& inFile, const std::string& outFile);
};
