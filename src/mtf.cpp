#include "mtf.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace std;

namespace mtf {

vector<uint8_t> encode(const vector<uint8_t>& input) {
    array<uint8_t, 256> symbols{};
    for (int i = 0; i < 256; ++i) {
        symbols[i] = static_cast<uint8_t>(i);
    }

    vector<uint8_t> output;
    output.reserve(input.size());

    for (uint8_t value : input) {
        int idx = 0;
        while (idx < 256 && symbols[idx] != value) {
            ++idx;
        }
        if (idx == 256) {
            throw runtime_error("MTF encode failed: symbol not found.");
        }

        output.push_back(static_cast<uint8_t>(idx));
        const uint8_t moved = symbols[idx];
        while (idx > 0) {
            symbols[idx] = symbols[idx - 1];
            --idx;
        }
        symbols[0] = moved;
    }

    return output;
}

vector<uint8_t> decode(const vector<uint8_t>& input) {
    array<uint8_t, 256> symbols{};
    for (int i = 0; i < 256; ++i) {
        symbols[i] = static_cast<uint8_t>(i);
    }

    vector<uint8_t> output;
    output.reserve(input.size());

    for (uint8_t index : input) {
        const int idx = static_cast<int>(index);
        if (idx < 0 || idx >= 256) {
            throw runtime_error("MTF decode failed: invalid index.");
        }

        const uint8_t value = symbols[idx];
        output.push_back(value);

        int cur = idx;
        while (cur > 0) {
            symbols[cur] = symbols[cur - 1];
            --cur;
        }
        symbols[0] = value;
    }

    return output;
}

}  // namespace mtf
