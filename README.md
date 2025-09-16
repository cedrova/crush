# Crush – Fast File Compression CLI

![GitHub stars](https://img.shields.io/github/stars/yourusername/crush?style=flat-square)
![GitHub forks](https://img.shields.io/github/forks/yourusername/crush?style=flat-square)
![GitHub license](https://img.shields.io/github/license/yourusername/crush?style=flat-square)
![C++](https://img.shields.io/badge/C++-17-blue?style=flat-square)

Crush is a lightweight, multi-algorithm file compression CLI supporting **Huffman**, **LZ77**, and **BWT**.

---

## Features

- Compression & Decompression
- Benchmark mode
- Multi-threaded
- Adjustable compression levels (1–9)
- Cross-platform (Windows, Linux, macOS)

---

## Installation

```bash
git clone https://github.com/yourusername/crush.git
cd crush
mkdir build
cd build
cmake ..
cmake --build . --config Debug
