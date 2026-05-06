#include "rle2.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace std;

namespace rle2 {

namespace {
constexpr uint8_t kEscape = 0xFF;
constexpr uint8_t kEscLiteral = 0x00;
constexpr uint8_t kEscRun = 0x01;
constexpr size_t kMinRun = 4;
}  // namespace

vector<uint8_t> encode(const vector<uint8_t>& input) {
    vector<uint8_t> output;
    output.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        size_t run = 1;
        while (i + run < input.size() && input[i + run] == input[i] && run < 255) {
            ++run;
        }

        const uint8_t value = input[i];
        if (run >= kMinRun) {
            output.push_back(kEscape);
            output.push_back(kEscRun);
            output.push_back(static_cast<uint8_t>(run));
            output.push_back(value);
        } else {
            for (size_t j = 0; j < run; ++j) {
                if (value == kEscape) {
                    output.push_back(kEscape);
                    output.push_back(kEscLiteral);
                } else {
                    output.push_back(value);
                }
            }
        }
        i += run;
    }

    return output;
}

vector<uint8_t> decode(const vector<uint8_t>& input) {
    vector<uint8_t> output;
    output.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        if (input[i] != kEscape) {
            output.push_back(input[i]);
            ++i;
            continue;
        }
        if (i + 1 >= input.size()) {
            throw runtime_error("Corrupted RLE-2 stream: escape at end.");
        }

        const uint8_t tag = input[i + 1];
        if (tag == kEscLiteral) {
            output.push_back(kEscape);
            i += 2;
            continue;
        }
        if (tag == kEscRun) {
            if (i + 3 >= input.size()) {
                throw runtime_error("Corrupted RLE-2 stream: truncated run.");
            }
            const uint8_t run = input[i + 2];
            const uint8_t value = input[i + 3];
            if (run == 0) {
                throw runtime_error("Corrupted RLE-2 stream: zero run.");
            }
            output.insert(output.end(), run, value);
            i += 4;
            continue;
        }

        throw runtime_error("Corrupted RLE-2 stream: unknown escape tag.");
    }

    return output;
}

}  // namespace rle2
