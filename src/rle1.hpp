#ifndef RLE1_HPP
#define RLE1_HPP

#include <cstdint>
#include <vector>

using namespace std;

namespace rle1 {

vector<uint8_t> encode(const vector<uint8_t>& input);
vector<uint8_t> decode(const vector<uint8_t>& input);

}  // namespace rle1

#endif
