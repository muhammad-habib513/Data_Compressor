# Data Compression Project  
## Simplified BZip2 Compression Algorithm Implementation

---

# Group Members

- Muhammad Habib (22L-6951)
- Shahjahan Sarfraz (22L-6960)
- Muhammad Shaheer (22L-6942)

---

# Project Overview

This project implements a simplified version of the BZip2 compression algorithm in C++. The implementation follows a modular pipeline architecture consisting of multiple compression stages including:

- Run-Length Encoding (RLE-1)
- Burrows-Wheeler Transform (BWT)
- Move-to-Front Transform (MTF)
- Run-Length Encoding (RLE-2)
- ANS-based Entropy Coding

The project supports both compression and decompression pipelines, configurable block sizes, benchmarking utilities, and automated testing.

---

# Compression Pipeline

```text
Input File
   ↓
Block Division
   ↓
RLE-1 Encoding
   ↓
BWT
   ↓
MTF
   ↓
RLE-2 Encoding
   ↓
ANS Entropy Coding
   ↓
Compressed Output
```

---

# Decompression Pipeline

```text
Compressed Input
   ↓
ANS Decoding
   ↓
RLE-2 Decoding
   ↓
Inverse MTF
   ↓
Inverse BWT
   ↓
RLE-1 Decoding
   ↓
Original Output
```

---

# Features

- Matrix-based Burrows-Wheeler Transform
- Inverse BWT
- Move-to-Front Encoding and Decoding
- RLE-1 and RLE-2 implementations
- ANS-based entropy coding
- Compression and decompression support
- Configurable block sizes
- Benchmarking support
- Unit testing
- Cross-platform Makefile support
- Modular C++ architecture

---

# Repository Structure

```text
project-bzip2/
│
├── src/
│   ├── main.cpp
│   ├── pipeline.cpp
│   ├── bwt.cpp
│   ├── mtf.cpp
│   ├── rle1.cpp
│   ├── rle2.cpp
│   ├── ans.cpp
│   └── tests.cpp
│
├── include/
│
├── benchmarks/
│
├── results/
│
├── Makefile
├── config.ini
├── README.md
└── report.pdf
```

---

# Build Instructions

## Linux

Compile the project:

```bash
make
```

Run the compressor:

```bash
./compressor
```

Run tests:

```bash
make test
```

Clean generated files:

```bash
make clean
```

---

# Usage

## Compression

```bash
./compressor encode input.txt compressed.bin 500
```

## Decompression

```bash
./compressor decode compressed.bin restored.txt
```

---

# Configuration

The project uses a `config.ini` file for configurable settings such as:

- Block size
- Benchmark mode
- Enabling/disabling stages
- Input/output directories

---

# Testing

Unit tests are included for:
- RLE-1
- BWT
- MTF
- RLE-2
- Pipeline validation

Run all tests using:

```bash
make test
```

---

# Benchmarking

The project supports benchmarking on:
- Text files
- Binary files
- Repetitive datasets

Results are generated in:

```text
results/results.csv
```

Performance graphs can also be generated using Python scripts.

---

# Extra Features Implemented

- ANS-based entropy coding
- Enhanced RLE stages
- Modular pipeline architecture
- Automated benchmarking support
- Cross-platform Makefile

---

# Notes

- The implementation uses a unique marker symbol during BWT transformation.
- Generated benchmark files are excluded using `.gitignore`.

---
