#include "interface.h"

#include <string>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
  return s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t sum = 0;
  std::vector<std::string> decoded;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.resize(codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
      decoded[i] = dictionary[static_cast<std::size_t>(codes[i])];
    }
    sum = 0;
    for (std::size_t i = 0; i < decoded.size(); ++i) {
      if (matches_prefix(decoded[i])) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}
