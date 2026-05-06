#ifndef RLE2_HPP
#define RLE2_HPP

#include <cstdint>
#include <vector>

using namespace std;

namespace rle2 {

vector<uint8_t> encode(const vector<uint8_t>& input);
vector<uint8_t> decode(const vector<uint8_t>& input);

}  // namespace rle2

#endif
