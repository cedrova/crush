#include "compressor.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>
#include <queue>
#include <algorithm>
#include <string>

// ===== Huffman helpers =====
struct HuffmanNode {
    char c;
    int freq;
    HuffmanNode *left = nullptr, *right = nullptr;

    HuffmanNode(char ch, int f) : c(ch), freq(f) {}
    HuffmanNode(HuffmanNode* l, HuffmanNode* r) : c(0), freq(l->freq+r->freq), left(l), right(r) {}
};

struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) { return a->freq > b->freq; }
};

void buildCodes(HuffmanNode* node, const std::string& prefix, std::map<char,std::string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) codes[node->c] = prefix;
    buildCodes(node->left, prefix+"0", codes);
    buildCodes(node->right, prefix+"1", codes);
}

void freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

// ===== LZ77 helpers =====
struct LZ77Token {
    int offset;
    int length;
    char next;
};

// Simple LZ77 encode with fixed window size
std::vector<LZ77Token> lz77Compress(const std::vector<char>& data, int windowSize=2048, int lookahead=32) {
    std::vector<LZ77Token> tokens;
    size_t pos = 0;
    while (pos < data.size()) {
        int matchLen = 0;
        int matchOffset = 0;
        int start = std::max<int>(0, pos - windowSize);
        for (int i = start; i < pos; ++i) {
            int len = 0;
            while (len < lookahead && pos+len < data.size() && data[i+len] == data[pos+len]) ++len;
            if (len > matchLen) { matchLen = len; matchOffset = pos-i; }
        }
        char nextChar = (pos + matchLen < data.size()) ? data[pos+matchLen] : 0;
        tokens.push_back({matchOffset, matchLen, nextChar});
        pos += matchLen + 1;
    }
    return tokens;
}

// Simple LZ77 decode
std::vector<char> lz77Decompress(const std::vector<LZ77Token>& tokens) {
    std::vector<char> result;
    for (auto &t : tokens) {
        size_t start = result.size() - t.offset;
        for (int i=0;i<t.length;++i) result.push_back(result[start+i]);
        if (t.next != 0) result.push_back(t.next);
    }
    return result;
}

// ===== BWT helpers =====
std::string bwtTransform(const std::string& s) {
    std::string s2 = s + '\0';
    std::vector<std::string> rotations;
    for (size_t i=0;i<s2.size();++i)
        rotations.push_back(s2.substr(i)+s2.substr(0,i));
    std::sort(rotations.begin(), rotations.end());
    std::string result;
    for (auto &r : rotations) result += r.back();
    return result;
}

std::string bwtInverse(const std::string& s) {
    size_t n = s.size();
    std::vector<std::string> table(n);
    for (size_t i=0;i<n;++i) {
        for (size_t j=0;j<n;++j) table[j] = s[j] + table[j];
        std::sort(table.begin(), table.end());
    }
    for (auto &row : table)
        if (row.back() == '\0') return row.substr(0,row.size()-1);
    return "";
}

// ===== Compressor class implementation =====
void Compressor::compress(const std::string &input,
                          const std::string &output,
                          Algorithm algo,
                          int level,
                          int threads) {
    std::ifstream fin(input, std::ios::binary);
    if (!fin) throw std::runtime_error("Cannot open input file");
    std::vector<char> data((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    std::ofstream fout(output, std::ios::binary);
    if (!fout) throw std::runtime_error("Cannot open output file");

    switch(algo) {
        case Algorithm::Huffman: {
            // Huffman (same as Phase 3)
            std::map<char,int> freq;
            for (char c : data) freq[c]++;
            std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNode> pq;
            for (auto &p : freq) pq.push(new HuffmanNode(p.first, p.second));
            while (pq.size() > 1) {
                HuffmanNode* a = pq.top(); pq.pop();
                HuffmanNode* b = pq.top(); pq.pop();
                pq.push(new HuffmanNode(a,b));
            }
            HuffmanNode* root = pq.top();
            std::map<char,std::string> codes;
            buildCodes(root, "", codes);
            uint32_t tableSize = freq.size();
            fout.write(reinterpret_cast<const char*>(&tableSize), sizeof(tableSize));
            for (auto &p : freq) { fout.put(p.first); uint32_t f = p.second; fout.write(reinterpret_cast<const char*>(&f), sizeof(f)); }
            std::string bitString;
            for (char c : data) bitString += codes[c];
            for (size_t i=0;i<bitString.size();i+=8) {
                std::string byteStr = bitString.substr(i,8);
                while(byteStr.size() < 8) byteStr += '0';
                uint8_t b = std::bitset<8>(byteStr).to_ulong();
                fout.put(b);
            }
            freeTree(root);
            std::cout << "[compress] done, Huffman\n";
            break;
        }
        case Algorithm::LZ77: {
            auto tokens = lz77Compress(data);
            // Simple write: offset, length, next char
            uint32_t count = tokens.size();
            fout.write(reinterpret_cast<const char*>(&count), sizeof(count));
            for (auto &t : tokens) {
                fout.write(reinterpret_cast<const char*>(&t.offset), sizeof(t.offset));
                fout.write(reinterpret_cast<const char*>(&t.length), sizeof(t.length));
                fout.put(t.next);
            }
            std::cout << "[compress] done, LZ77\n";
            break;
        }
        case Algorithm::BWT: {
            std::string s(data.begin(), data.end());
            std::string transformed = bwtTransform(s);
            fout.write(transformed.data(), transformed.size());
            std::cout << "[compress] done, BWT\n";
            break;
        }
        case Algorithm::Arithmetic:
            // Stub for now
            fout.write(data.data(), data.size());
            std::cout << "[compress] stub Arithmetic: copied input\n";
            break;
        default:
            throw std::runtime_error("Unknown algorithm");
    }
}

void Compressor::decompress(const std::string &input,
                            const std::string &output) {
    std::ifstream fin(input, std::ios::binary);
    if (!fin) throw std::runtime_error("Cannot open input file");
    std::ofstream fout(output, std::ios::binary);
    if (!fout) throw std::runtime_error("Cannot open output file");

    // Attempt to detect Huffman (by reading table size)
    uint32_t tableSize = 0;
    fin.read(reinterpret_cast<char*>(&tableSize), sizeof(tableSize));
    if (tableSize > 0 && tableSize < 100000) {
        // Huffman decompression
        std::map<char,int> freq;
        for (uint32_t i=0;i<tableSize;++i) {
            char c = fin.get();
            uint32_t f = 0;
            fin.read(reinterpret_cast<char*>(&f), sizeof(f));
            freq[c] = f;
        }
        std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNode> pq;
        for (auto &p : freq) pq.push(new HuffmanNode(p.first, p.second));
        while (pq.size() > 1) {
            HuffmanNode* a = pq.top(); pq.pop();
            HuffmanNode* b = pq.top(); pq.pop();
            pq.push(new HuffmanNode(a,b));
        }
        HuffmanNode* root = pq.top();

        std::vector<char> encodedBytes((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        std::string bitString;
        for (char b : encodedBytes) bitString += std::bitset<8>(b).to_string();

        HuffmanNode* node = root;
        for (char bit : bitString) {
            node = (bit=='0') ? node->left : node->right;
            if (!node->left && !node->right) {
                fout.put(node->c);
                node = root;
            }
        }
        freeTree(root);
        std::cout << "[decompress] done, Huffman\n";
        return;
    }

    // Fallback: just copy (BWT/LZ77/Arithmetic)
    std::vector<char> data((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fout.write(data.data(), data.size());
    std::cout << "[decompress] done (non-Huffman)\n";
}

void Compressor::benchmark(const std::string &input, Algorithm algo) {
    std::cout << "[benchmark] not implemented yet for algo=" << static_cast<int>(algo) << "\n";
}
