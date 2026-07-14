#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Precompute which dictionary entries match the prefix rule.
  std::vector<uint8_t> match(dictionary.size(), 0);
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    if (s[0] == 'q' && s[1] <= 'm') {
      match[i] = 1;
    }
  }

  uint64_t sum = 0;
  const std::size_t n = codes.size();
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* match_ptr = match.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(weights_ptr[i]) * static_cast<uint64_t>(match_ptr[codes_ptr[i]]);
    }
  }
  return sum;
}
