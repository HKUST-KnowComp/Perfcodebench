#include "interface.h"

#include <string>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
  return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < codes.size(); ++i) {
      const std::string& decoded = dictionary[static_cast<std::size_t>(codes[i])];
      if (matches_prefix(decoded)) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}