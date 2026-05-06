# Data Compression Project

## Group Members
- Muhammad Habib (22L-6951)
- Shahjahan Sarfraz (22L-6960)
- Muhammad Shaheer (22L-6942)

---

# Overview

This project implements a modular data compression pipeline in C++ as part of the Data Compression course project.

The project is based on **Track B**:
- Burrows-Wheeler Transform (BWT)
- Move-To-Front Encoding (MTF)
- Arithmetic Coding

To further improve compression performance, additional preprocessing stages such as Run-Length Encoding (RLE) were integrated into the pipeline.

The implementation includes:
- Compression and decompression support
- Benchmarking utilities
- Testing framework
- Makefile-based build system

---

# Compression Pipeline

Current Stage 3 pipeline:

```text
Input File
   ↓
RLE-1
   ↓
BWT
   ↓
MTF
   ↓
RLE-2
   ↓
ANS
   ↓
Compressed Output
```

The decompression pipeline applies the reverse operations to reconstruct the original input file.

---

# Features

- Burrows-Wheeler Transform (BWT)
- Inverse BWT
- Move-To-Front Encoding (MTF)
- ANS Entropy Coding
- Run-Length Encoding (RLE)
- File compression and decompression
- Benchmarking support
- Unit testing
- Modular C++ implementation
- Makefile support
