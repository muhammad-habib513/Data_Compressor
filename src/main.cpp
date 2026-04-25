#include "pipeline.hpp"

#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

namespace {

constexpr size_t kMinBlockKB = 100;
constexpr size_t kMaxBlockKB = 900;

void print_usage() {
    cout
        << "Usage:\n"
        << "  compressor encode <input_file> <output_file> <block_size_kb>\n"
        << "  compressor decode <input_file> <output_file>\n\n"
        << "Stage 1 encode pipeline: RLE-1 then BWT\n"
        << "Block size range: 100KB to 900KB\n";
}

void print_stats(const string& action, const pipeline::Stats& stats) {
    cout << action << " completed.\n";
    cout << "  Blocks: " << stats.blocks << "\n";
    cout << "  Input bytes: " << stats.input_bytes << "\n";
    cout << "  Output bytes: " << stats.output_bytes << "\n";

    if (stats.input_bytes > 0) {
        const double ratio = static_cast<double>(stats.output_bytes) / static_cast<double>(stats.input_bytes);
        cout << "  Ratio (output/input): " << fixed << setprecision(4) << ratio << "\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            print_usage();
            return 1;
        }

        const string mode = argv[1];
        if (mode == "encode") {
            if (argc != 5) {
                print_usage();
                return 1;
            }
            const size_t block_kb = static_cast<size_t>(stoul(argv[4]));
            if (block_kb < kMinBlockKB || block_kb > kMaxBlockKB) {
                cerr << "Error: block size must be between 100 and 900 KB.\n";
                return 1;
            }

            const size_t block_size = block_kb * 1024;
            const pipeline::Stats stats = pipeline::compress_file(argv[2], argv[3], block_size);
            print_stats("Encoding", stats);
            return 0;
        }

        if (mode == "decode") {
            if (argc != 4) {
                print_usage();
                return 1;
            }
            const pipeline::Stats stats = pipeline::decompress_file(argv[2], argv[3]);
            print_stats("Decoding", stats);
            return 0;
        }

        print_usage();
        return 1;
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
