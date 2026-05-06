#include "ans.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace std;

namespace ans {

namespace {

constexpr uint32_t kScaleBits = 12;
constexpr uint32_t kTotFreq = 1u << kScaleBits;  // 4096
constexpr uint32_t kRansL = 1u << 23;

void push_u16(vector<uint8_t>& out, uint16_t v) {
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
}

void push_u32(vector<uint8_t>& out, uint32_t v) {
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
}

uint16_t read_u16(const vector<uint8_t>& in, size_t& pos) {
    if (pos + 2 > in.size()) {
        throw runtime_error("ANS decode failed: truncated u16.");
    }
    const uint16_t v = static_cast<uint16_t>(in[pos]) |
                       (static_cast<uint16_t>(in[pos + 1]) << 8);
    pos += 2;
    return v;
}

uint32_t read_u32(const vector<uint8_t>& in, size_t& pos) {
    if (pos + 4 > in.size()) {
        throw runtime_error("ANS decode failed: truncated u32.");
    }
    const uint32_t v = static_cast<uint32_t>(in[pos]) |
                       (static_cast<uint32_t>(in[pos + 1]) << 8) |
                       (static_cast<uint32_t>(in[pos + 2]) << 16) |
                       (static_cast<uint32_t>(in[pos + 3]) << 24);
    pos += 4;
    return v;
}

array<uint16_t, 256> normalize_frequencies(const vector<uint8_t>& input) {
    array<uint32_t, 256> counts{};
    for (uint8_t b : input) {
        ++counts[b];
    }

    array<uint16_t, 256> freq{};
    uint32_t sum = 0;
    vector<pair<uint32_t, int>> used;

    for (int s = 0; s < 256; ++s) {
        if (counts[s] == 0) {
            continue;
        }
        uint64_t scaled = (static_cast<uint64_t>(counts[s]) * kTotFreq) / input.size();
        if (scaled == 0) {
            scaled = 1;
        }
        freq[s] = static_cast<uint16_t>(scaled);
        sum += freq[s];
        used.push_back({counts[s], s});
    }

    if (used.empty()) {
        return freq;
    }

    sort(used.begin(), used.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

    while (sum < kTotFreq) {
        for (const auto& item : used) {
            if (sum == kTotFreq) {
                break;
            }
            ++freq[item.second];
            ++sum;
        }
    }
    while (sum > kTotFreq) {
        for (auto it = used.rbegin(); it != used.rend() && sum > kTotFreq; ++it) {
            if (freq[it->second] > 1) {
                --freq[it->second];
                --sum;
            }
        }
    }

    return freq;
}

}  // namespace

vector<uint8_t> encode(const vector<uint8_t>& input) {
    vector<uint8_t> out;
    out.reserve(input.size() + 600);

    push_u32(out, static_cast<uint32_t>(input.size()));
    if (input.empty()) {
        push_u16(out, 0);
        return out;
    }

    const array<uint16_t, 256> freq = normalize_frequencies(input);
    array<uint32_t, 256> cum{};
    uint32_t running = 0;
    uint16_t used_count = 0;
    for (int s = 0; s < 256; ++s) {
        cum[s] = running;
        running += freq[s];
        if (freq[s] > 0) {
            ++used_count;
        }
    }
    if (running != kTotFreq) {
        throw runtime_error("ANS encode failed: invalid normalized frequencies.");
    }

    push_u16(out, used_count);
    for (int s = 0; s < 256; ++s) {
        if (freq[s] == 0) {
            continue;
        }
        out.push_back(static_cast<uint8_t>(s));
        push_u16(out, freq[s]);
    }

    uint32_t state = kRansL;
    vector<uint8_t> renorm_bytes;
    renorm_bytes.reserve(input.size());

    for (size_t idx = input.size(); idx > 0; --idx) {
        const uint8_t sym = input[idx - 1];
        const uint32_t f = freq[sym];
        const uint32_t c = cum[sym];

        const uint32_t x_max = ((kRansL >> kScaleBits) << 8) * f;
        while (state >= x_max) {
            renorm_bytes.push_back(static_cast<uint8_t>(state & 0xFF));
            state >>= 8;
        }

        state = ((state / f) << kScaleBits) + (state % f) + c;
    }

    push_u32(out, state);
    reverse(renorm_bytes.begin(), renorm_bytes.end());
    out.insert(out.end(), renorm_bytes.begin(), renorm_bytes.end());
    return out;
}

vector<uint8_t> decode(const vector<uint8_t>& input) {
    size_t pos = 0;
    const uint32_t output_size = read_u32(input, pos);
    const uint16_t used_count = read_u16(input, pos);
    if (output_size == 0) {
        return {};
    }
    if (used_count == 0) {
        throw runtime_error("ANS decode failed: zero symbols for non-empty output.");
    }

    array<uint16_t, 256> freq{};
    for (uint16_t i = 0; i < used_count; ++i) {
        if (pos >= input.size()) {
            throw runtime_error("ANS decode failed: truncated frequency table.");
        }
        const uint8_t sym = input[pos++];
        const uint16_t f = read_u16(input, pos);
        if (f == 0) {
            throw runtime_error("ANS decode failed: zero symbol frequency.");
        }
        freq[sym] = f;
    }

    array<uint32_t, 256> cum{};
    uint32_t running = 0;
    for (int s = 0; s < 256; ++s) {
        cum[s] = running;
        running += freq[s];
    }
    if (running != kTotFreq) {
        throw runtime_error("ANS decode failed: invalid total frequency.");
    }

    if (pos + 4 > input.size()) {
        throw runtime_error("ANS decode failed: missing final state.");
    }
    uint32_t state = read_u32(input, pos);

    array<uint8_t, kTotFreq> symbol_by_slot{};
    for (int s = 0; s < 256; ++s) {
        for (uint32_t i = 0; i < freq[s]; ++i) {
            symbol_by_slot[cum[s] + i] = static_cast<uint8_t>(s);
        }
    }

    vector<uint8_t> output(output_size);
    size_t read_pos = pos;
    for (uint32_t i = 0; i < output_size; ++i) {
        const uint32_t slot = state & (kTotFreq - 1);
        const uint8_t sym = symbol_by_slot[slot];
        output[i] = sym;

        state = freq[sym] * (state >> kScaleBits) + (slot - cum[sym]);
        while (state < kRansL) {
            if (read_pos >= input.size()) {
                throw runtime_error("ANS decode failed: truncated renormalization bytes.");
            }
            state = (state << 8) | input[read_pos++];
        }
    }

    return output;
}

}  // namespace ans
