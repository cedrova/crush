# Crush – Fast File Compression CLI

![GitHub stars](https://img.shields.io/github/stars/yourusername/crush?style=flat-square)
![GitHub forks](https://img.shields.io/github/forks/yourusername/crush?style=flat-square)
![GitHub license](https://img.shields.io/github/license/yourusername/crush?style=flat-square)
![C++](https://img.shields.io/badge/C++-17-blue?style=flat-square)

**Crush** is a lightweight, fast, multi-algorithm file compression tool.  
It supports **Huffman Coding**, **LZ77**, and **Burrows–Wheeler Transform (BWT)** with a command-line interface (CLI), benchmarking, and optional multithreading.

---

## Features

- **Compression & Decompression**
  - Multiple algorithms: Huffman, LZ77, BWT
  - Adjustable compression levels (1–9)
  - Multi-threaded compression support
- **Benchmark Mode**
  - Test input files for speed and compression efficiency
  - Compare different algorithms
- **Cross-platform**
  - Works on Windows, Linux, and macOS (requires C++17)
- **Phase-wise Architecture**
  - Phase 1: CLI skeleton and argument parsing
  - Phase 2: Huffman compression
  - Phase 3: Full multi-algorithm support

---

## Installation

1. **Clone the repository**

```bash
git clone https://github.com/yourusername/crush.git
cd crush
