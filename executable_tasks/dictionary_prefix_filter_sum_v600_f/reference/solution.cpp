#include "interface.h"

#include <cstdint>
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
  std::vector<uint8_t> accepted(dictionary.size(), 0);
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    accepted[i] = matches_prefix(dictionary[i]) ? 1u : 0u;
  }
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < codes.size(); ++i) {
      if (accepted[static_cast<std::size_t>(codes[i])]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}
