#ifndef BWT_HPP
#define BWT_HPP

#include <cstdint>
#include <utility>
#include <vector>

using namespace std;

namespace bwt {

pair<vector<uint8_t>, uint32_t> transform(
    const vector<uint8_t>& input
);
vector<uint8_t> inverse_transform(
    const vector<uint8_t>& input, uint32_t primary_index
);

}  // namespace bwt

#endif
