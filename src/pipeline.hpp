#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <cstddef>
#include <cstdint>
#include <string>

using namespace std;

namespace pipeline {

struct Stats {
    size_t input_bytes = 0;
    size_t output_bytes = 0;
    size_t blocks = 0;
};

Stats compress_file(
    const string& input_path, const string& output_path, size_t block_size
);
Stats decompress_file(const string& input_path, const string& output_path);

}  // namespace pipeline

#endif
