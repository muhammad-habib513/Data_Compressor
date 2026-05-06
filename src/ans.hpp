#ifndef ANS_HPP
#define ANS_HPP

#include <cstdint>
#include <vector>

using namespace std;

namespace ans {

vector<uint8_t> encode(const vector<uint8_t>& input);
vector<uint8_t> decode(const vector<uint8_t>& input);

}  // namespace ans

#endif
