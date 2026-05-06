# Data Compression Project

## Group Members
- Muhammad Habib (22L-6951)
- Shahjahan Sarfraz (22L-6960)

---

# Overview

This project implements a modular data compression pipeline in C++ as part of the Data Compression course project.

The project follows **Track B**:
- Burrows-Wheeler Transform (BWT)
- Move-To-Front Encoding (MTF)
- Arithmetic Coding

The implementation is divided into multiple stages including transformation, encoding, decoding, benchmarking, and performance analysis.

---

# Compression Pipeline

Current pipeline structure:

```text
Input File
   ↓
BWT
   ↓
MTF
   ↓
Arithmetic Coding
   ↓
Compressed Output
