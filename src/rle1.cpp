#include "rle1.hpp"

#include <stdexcept>

using namespace std;

namespace rle1 {

namespace {
constexpr uint8_t kMarker = 0xFF;
constexpr size_t kMinRunLength = 4;
}  // namespace

vector<uint8_t> encode(const vector<uint8_t>& input) {
    vector<uint8_t> output;
    output.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        size_t run_len = 1;
        while (i + run_len < input.size() && input[i + run_len] == input[i] && run_len < 255) {
            ++run_len;
        }

        const uint8_t value = input[i];
        if (run_len >= kMinRunLength || value == kMarker) {
            output.push_back(kMarker);
            output.push_back(static_cast<uint8_t>(run_len));
            output.push_back(value);
        } else {
            for (size_t j = 0; j < run_len; ++j) {
                output.push_back(value);
            }
        }

        i += run_len;
    }

    return output;
}

vector<uint8_t> decode(const vector<uint8_t>& input) {
    vector<uint8_t> output;
    output.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        if (input[i] != kMarker) {
            output.push_back(input[i]);
            ++i;
            continue;
        }

        if (i + 2 >= input.size()) {
            throw runtime_error("Corrupted RLE data: truncated marker sequence.");
        }

        const uint8_t run_len = input[i + 1];
        const uint8_t value = input[i + 2];
        if (run_len == 0) {
            throw runtime_error("Corrupted RLE data: zero run length.");
        }

        output.insert(output.end(), run_len, value);
        i += 3;
    }

    return output;
}

}  // namespace rle1
