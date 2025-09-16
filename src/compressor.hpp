#pragma once
#include <string>

enum class Algorithm { Huffman, LZ77, BWT, Arithmetic, None };

class Compressor {
public:
    void compress(const std::string &input,
                  const std::string &output,
                  Algorithm algo,
                  int level = 5,
                  int threads = 1);

    void decompress(const std::string &input,
                    const std::string &output);

    void benchmark(const std::string &input,
                   Algorithm algo);
};
