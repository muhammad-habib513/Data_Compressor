#ifndef MTF_HPP
#define MTF_HPP

#include <cstdint>
#include <vector>

using namespace std;

namespace mtf {

vector<uint8_t> encode(const vector<uint8_t>& input);
vector<uint8_t> decode(const vector<uint8_t>& input);

}  // namespace mtf

#endif
