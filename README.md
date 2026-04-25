# Data Compression Project - Stage 1 (C++)

This project implements Stage 1 in C++ with a reversible flow:

- Encode: `A -> A'`
- Decode: `A' -> A`

Implemented Stage-1 pipeline order:

- `RLE-1` first
- then `BWT`

## 3.1 Block Division and File Handling

### 3.1.1 Requirements

- Input file is processed in blocks.
- Block size is user-provided in KB, restricted to `100KB` to `900KB`.
- Archive format stores enough metadata to decode back exactly.
- Works in both directions (`encode` and `decode`).

### 3.1.2 Data Structures

- `vector<uint8_t>`: in-memory block buffer and payload buffer.
- `pipeline::Stats`: stores block count, input bytes, output bytes.
- Archive header and per-block records:
  - Magic bytes: `DCP3`
  - Global block size
  - Algorithm id (`rle1` or `bwt`)
  - Per block: original size, encoded size, BWT primary index

### 3.1.3 Function Prototypes

- `pipeline::Stats compress_file(const string& input_path, const string& output_path, size_t block_size);`
- `pipeline::Stats decompress_file(const string& input_path, const string& output_path);`

## Build

```bash
make
```

This builds:

- `compressor` (main program)
- `tests` (unit/integration tests)

## 3.2 RLE-1 Implementation

### 3.2.1 Description

- Uses marker-based run-length encoding.
- Long runs are encoded as marker + run-length + byte.
- Decoder expands runs to recover the exact original bytes (after inverse BWT in full pipeline).

### 3.2.2 Function Prototypes

- `vector<uint8_t> rle1::encode(const vector<uint8_t>& input);`
- `vector<uint8_t> rle1::decode(const vector<uint8_t>& input);`

## 3.3 Burrows-Wheeler Transform (BWT)

### 3.3.1 Description

- BWT transform rearranges bytes into a permutation that is reversible.
- In Stage 1 pipeline, BWT is applied on the output of RLE-1.
- Encoder stores the BWT `primary_index` per block.
- Decoder first applies inverse BWT using `primary_index`, then runs RLE-1 decode.

### 3.3.2 Data Structures

- `vector<int>` for suffix/cyclic-shift arrays (`p`, `c`, etc.).
- `array<uint32_t, 256>` for counting and LF-mapping support.
- `vector<uint8_t>` for transformed output and restored output.

### 3.3.3 Function Prototypes

- `pair<vector<uint8_t>, uint32_t> bwt::transform(const vector<uint8_t>& input);`
- `vector<uint8_t> bwt::inverse_transform(const vector<uint8_t>& input, uint32_t primary_index);`

## Usage

### Encode

```bash
./compressor encode <input_file> <output_file> <block_size_kb>
```

- `block_size_kb` must be between `100` and `900`.
- Fixed Stage 1 order is used: `RLE-1 -> BWT`.

### Decode

```bash
./compressor decode <input_file> <output_file>
```

## 3.4 Stage 1 Evaluation

- CLI prints:
  - Total blocks
  - Input bytes
  - Output bytes
  - Output/Input ratio
- Unit tests verify:
  - RLE roundtrip
  - BWT roundtrip
  - End-to-end file roundtrip for complete Stage 1 order (`RLE-1 -> BWT`)

## Tests

```bash
make test
```

Includes:

- RLE round-trip unit test
- BWT round-trip unit test
- End-to-end file round-trip test for full Stage 1 pipeline

## Format Notes

- Custom binary container format with magic header `DCP3`
- Stores per-block metadata:
  - original block size
  - encoded block size
  - BWT primary index

## Example

```bash
./compressor encode data.txt data.dcp 200
./compressor decode data.dcp restored.txt
cmp data.txt restored.txt
```

If `cmp` shows no output, decoding restored the original file exactly.
