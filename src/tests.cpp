#include "ans.hpp"
#include "bwt.hpp"
#include "mtf.hpp"
#include "pipeline.hpp"
#include "rle1.hpp"
#include "rle2.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace {

void expect_true(bool cond, const string& message) {
    if (!cond) {
        throw runtime_error("Test failed: " + message);
    }
}

void test_rle_roundtrip() {
    vector<uint8_t> data = {'A', 'A', 'A', 'A', 'B', 'C', 'C', 'C', 'C', 'C', 0xFF, 0xFF};
    vector<uint8_t> enc = rle1::encode(data);
    vector<uint8_t> dec = rle1::decode(enc);
    expect_true(dec == data, "RLE roundtrip mismatch");
}

void test_bwt_roundtrip() {
    vector<uint8_t> data = {'b', 'a', 'n', 'a', 'n', 'a', '_', 'b', 'w', 't'};
    auto transformed = bwt::transform(data);
    vector<uint8_t> restored = bwt::inverse_transform(transformed.first, transformed.second);
    expect_true(restored == data, "BWT roundtrip mismatch");
}

void test_mtf_roundtrip() {
    vector<uint8_t> data;
    for (int i = 0; i < 2000; ++i) {
        data.push_back(static_cast<uint8_t>((i * 17) % 256));
        if ((i % 7) == 0) {
            data.push_back(static_cast<uint8_t>('A'));
            data.push_back(static_cast<uint8_t>('A'));
            data.push_back(static_cast<uint8_t>('A'));
        }
    }
    vector<uint8_t> enc = mtf::encode(data);
    vector<uint8_t> dec = mtf::decode(enc);
    expect_true(dec == data, "MTF roundtrip mismatch");
}

void test_rle2_roundtrip() {
    vector<uint8_t> data = {
        0, 0, 0, 0, 0, 1, 2, 3, 3, 3, 3, 255, 255, 255, 255, 10, 11, 12, 12, 12, 12, 12
    };
    vector<uint8_t> enc = rle2::encode(data);
    vector<uint8_t> dec = rle2::decode(enc);
    expect_true(dec == data, "RLE-2 roundtrip mismatch");
}

void test_ans_roundtrip() {
    vector<uint8_t> data;
    data.reserve(5000);
    for (int i = 0; i < 5000; ++i) {
        data.push_back(static_cast<uint8_t>((i * 31 + (i / 7)) % 256));
        if ((i % 9) == 0) {
            data.push_back(0);
            data.push_back(0);
            data.push_back(0);
        }
    }
    vector<uint8_t> enc = ans::encode(data);
    vector<uint8_t> dec = ans::decode(enc);
    expect_true(dec == data, "ANS roundtrip mismatch");
}

void write_payload_file(const string& file_name, const vector<uint8_t>& payload) {
    ofstream out(file_name, ios::binary);
    expect_true(static_cast<bool>(out), "Cannot create test input file");
    out.write(reinterpret_cast<const char*>(payload.data()), static_cast<streamsize>(payload.size()));
}

void test_file_pipeline_roundtrip() {
    const string input_name = "test_input_stage1.bin";
    const string archive_name = "test_archive_stage1.dcp";
    const string output_name = "test_output_stage1.bin";

    vector<uint8_t> pattern;
    pattern.reserve(8192);
    for (int i = 0; i < 256; ++i) {
        pattern.push_back(static_cast<uint8_t>(i));
    }
    vector<uint8_t> payload;
    for (int i = 0; i < 20; ++i) {
        payload.insert(payload.end(), pattern.begin(), pattern.end());
    }

    write_payload_file(input_name, payload);

    pipeline::Stats enc_stats = pipeline::compress_file(input_name, archive_name, 100 * 1024);
    pipeline::Stats dec_stats = pipeline::decompress_file(archive_name, output_name);
    expect_true(enc_stats.input_bytes == payload.size(), "Encoded input size mismatch");
    expect_true(dec_stats.output_bytes == payload.size(), "Decoded output size mismatch");

    vector<uint8_t> restored(payload.size());
    {
        ifstream in(output_name, ios::binary);
        expect_true(static_cast<bool>(in), "Cannot open decoded output file");
        in.read(reinterpret_cast<char*>(restored.data()), static_cast<streamsize>(restored.size()));
        expect_true(in.gcount() == static_cast<streamsize>(restored.size()), "Decoded bytes count mismatch");
    }
    expect_true(restored == payload, "File pipeline roundtrip mismatch");
}

}  // namespace

int main() {
    try {
        test_rle_roundtrip();
        test_bwt_roundtrip();
        test_mtf_roundtrip();
        test_rle2_roundtrip();
        test_ans_roundtrip();
        test_file_pipeline_roundtrip();
        cout << "All tests passed.\n";
        return 0;
    } catch (const exception& ex) {
        cerr << ex.what() << "\n";
        return 1;
    }
}
