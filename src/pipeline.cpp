#include "pipeline.hpp"

#include "ans.hpp"
#include "bwt.hpp"
#include "mtf.hpp"
#include "rle1.hpp"
#include "rle2.hpp"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace pipeline {

namespace {

constexpr char kMagic[4] = {'D', 'C', 'P', '6'};
constexpr size_t kHeaderSize = 4 + 4;

void write_u32(ofstream& out, uint32_t value) {
    out.put(static_cast<char>(value & 0xFF));
    out.put(static_cast<char>((value >> 8) & 0xFF));
    out.put(static_cast<char>((value >> 16) & 0xFF));
    out.put(static_cast<char>((value >> 24) & 0xFF));
}

uint32_t read_u32(ifstream& in) {
    uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        int ch = in.get();
        if (ch == EOF) {
            throw runtime_error("Unexpected EOF while reading header.");
        }
        value |= (static_cast<uint32_t>(static_cast<unsigned char>(ch)) << (8 * i));
    }
    return value;
}

void ensure_read_ok(const ifstream& in, const string& message) {
    if (!in) {
        throw runtime_error(message);
    }
}

}  // namespace

Stats compress_file(const string& input_path, const string& output_path, size_t block_size) {
    ifstream input(input_path, ios::binary);
    if (!input) {
        throw runtime_error("Cannot open input file: " + input_path);
    }
    ofstream output(output_path, ios::binary);
    if (!output) {
        throw runtime_error("Cannot open output file: " + output_path);
    }

    output.write(kMagic, 4);
    write_u32(output, static_cast<uint32_t>(block_size));

    Stats stats{};
    stats.output_bytes += kHeaderSize;

    vector<uint8_t> block(block_size);
    while (true) {
        input.read(reinterpret_cast<char*>(block.data()), static_cast<streamsize>(block_size));
        const streamsize got = input.gcount();
        if (got <= 0) {
            break;
        }

        vector<uint8_t> raw(block.begin(), block.begin() + got);
        vector<uint8_t> rle_encoded = rle1::encode(raw);
        auto bwt_res = bwt::transform(rle_encoded);
        vector<uint8_t> mtf_encoded = mtf::encode(bwt_res.first);
        vector<uint8_t> rle2_encoded = rle2::encode(mtf_encoded);
        vector<uint8_t> encoded = ans::encode(rle2_encoded);
        uint32_t primary_index = bwt_res.second;

        write_u32(output, static_cast<uint32_t>(got));
        write_u32(output, static_cast<uint32_t>(encoded.size()));
        write_u32(output, primary_index);
        if (!encoded.empty()) {
            output.write(reinterpret_cast<const char*>(encoded.data()), static_cast<streamsize>(encoded.size()));
        }

        stats.input_bytes += static_cast<size_t>(got);
        stats.output_bytes += 12 + encoded.size();
        ++stats.blocks;
    }

    if (!input.eof()) {
        throw runtime_error("Error while reading input file.");
    }
    if (!output) {
        throw runtime_error("Error while writing output file.");
    }

    return stats;
}

Stats decompress_file(const string& input_path, const string& output_path) {
    ifstream input(input_path, ios::binary);
    if (!input) {
        throw runtime_error("Cannot open input file: " + input_path);
    }
    ofstream output(output_path, ios::binary);
    if (!output) {
        throw runtime_error("Cannot open output file: " + output_path);
    }

    char magic[4]{};
    input.read(magic, 4);
    ensure_read_ok(input, "Failed to read archive magic.");
    if (magic[0] != kMagic[0] || magic[1] != kMagic[1] || magic[2] != kMagic[2] || magic[3] != kMagic[3]) {
        throw runtime_error("Invalid archive format.");
    }

    (void)read_u32(input);  // Stored block size (metadata for reference).
    Stats stats{};
    stats.input_bytes += kHeaderSize;

    while (input.peek() != EOF) {
        const uint32_t original_size = read_u32(input);
        const uint32_t encoded_size = read_u32(input);
        const uint32_t primary_index = read_u32(input);

        vector<uint8_t> encoded(encoded_size);
        if (encoded_size > 0) {
            input.read(reinterpret_cast<char*>(encoded.data()), encoded_size);
            ensure_read_ok(input, "Failed to read encoded block payload.");
        }

        vector<uint8_t> rle2_encoded = ans::decode(encoded);
        vector<uint8_t> mtf_encoded = rle2::decode(rle2_encoded);
        vector<uint8_t> bwt_last = mtf::decode(mtf_encoded);
        vector<uint8_t> rle_encoded = bwt::inverse_transform(bwt_last, primary_index);
        vector<uint8_t> raw = rle1::decode(rle_encoded);

        if (raw.size() != original_size) {
            throw runtime_error("Corrupted archive: block size mismatch.");
        }

        if (!raw.empty()) {
            output.write(reinterpret_cast<const char*>(raw.data()), static_cast<streamsize>(raw.size()));
        }

        stats.input_bytes += 12 + encoded_size;
        stats.output_bytes += raw.size();
        ++stats.blocks;
    }

    if (!output) {
        throw runtime_error("Error while writing output file.");
    }

    return stats;
}

}  // namespace pipeline
