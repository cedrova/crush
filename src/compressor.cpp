#include "compressor.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <stdexcept>

// Node for Huffman tree
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Free tree memory
static void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

static void buildCodes(Node* root,
                       const std::string& str,
                       std::unordered_map<char, std::string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->ch] = str;
    }
    buildCodes(root->left, str + "0", codes);
    buildCodes(root->right, str + "1", codes);
}

void Compressor::compress(const std::string& inFile,
                          const std::string& outFile,
                          Algorithm algo) {
    if (algo == Algorithm::Huffman) {
        std::ifstream in(inFile, std::ios::binary);
        if (!in) throw std::runtime_error("Cannot open input file");
        std::string data((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
        in.close();

        std::unordered_map<char, int> freq;
        for (char c : data) freq[c]++;

        std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
        for (auto& kv : freq) pq.push(new Node(kv.first, kv.second));

        while (pq.size() > 1) {
            Node* l = pq.top(); pq.pop();
            Node* r = pq.top(); pq.pop();
            Node* m = new Node('\0', l->freq + r->freq);
            m->left = l;
            m->right = r;
            pq.push(m);
        }

        Node* root = pq.top();
        std::unordered_map<char, std::string> codes;
        buildCodes(root, "", codes);

        std::string encoded;
        for (char c : data) encoded += codes[c];

        std::ofstream out(outFile, std::ios::binary);
        if (!out) throw std::runtime_error("Cannot open output file");

        // Save frequencies for decompression
        size_t mapSize = freq.size();
        out.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
        for (auto& kv : freq) {
            out.write(&kv.first, sizeof(kv.first));
            out.write(reinterpret_cast<const char*>(&kv.second), sizeof(kv.second));
        }

        size_t bitSize = encoded.size();
        out.write(reinterpret_cast<const char*>(&bitSize), sizeof(bitSize));

        char buffer = 0;
        int count = 0;
        for (char bit : encoded) {
            buffer = (buffer << 1) | (bit - '0');
            count++;
            if (count == 8) {
                out.write(&buffer, 1);
                buffer = 0;
                count = 0;
            }
        }
        if (count > 0) {
            buffer <<= (8 - count);
            out.write(&buffer, 1);
        }

        out.close();
        freeTree(root);
    } else {
        throw std::runtime_error("Only Huffman implemented in Phase 2");
    }
}

void Compressor::decompress(const std::string& inFile,
                            const std::string& outFile) {
    std::ifstream in(inFile, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open input file");

    size_t mapSize;
    in.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
    std::unordered_map<char, int> freq;
    for (size_t i = 0; i < mapSize; i++) {
        char c;
        int f;
        in.read(&c, sizeof(c));
        in.read(reinterpret_cast<char*>(&f), sizeof(f));
        freq[c] = f;
    }

    size_t bitSize;
    in.read(reinterpret_cast<char*>(&bitSize), sizeof(bitSize));

    std::string bits;
    char byte;
    while (in.read(&byte, 1)) {
        for (int i = 7; i >= 0; i--) {
            bits.push_back(((byte >> i) & 1) + '0');
        }
    }
    in.close();
    bits = bits.substr(0, bitSize);

    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for (auto& kv : freq) pq.push(new Node(kv.first, kv.second));
    while (pq.size() > 1) {
        Node* l = pq.top(); pq.pop();
        Node* r = pq.top(); pq.pop();
        Node* m = new Node('\0', l->freq + r->freq);
        m->left = l;
        m->right = r;
        pq.push(m);
    }
    Node* root = pq.top();

    std::ofstream out(outFile, std::ios::binary);
    Node* cur = root;
    for (char bit : bits) {
        cur = (bit == '0') ? cur->left : cur->right;
        if (!cur->left && !cur->right) {
            out.put(cur->ch);
            cur = root;
        }
    }
    out.close();

    freeTree(root);
}
