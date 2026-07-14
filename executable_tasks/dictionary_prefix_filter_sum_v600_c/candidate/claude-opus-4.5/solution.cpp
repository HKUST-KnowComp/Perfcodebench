#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {

  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();

  // Precompute which dictionary entries match the prefix rule
  std::vector<uint8_t> dict_match(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    // matches_prefix: s[0] == 'q' && s[1] <= 'm'
    // Assuming strings have at least 2 chars based on original code
    dict_match[i] = (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') ? 1 : 0;
  }

  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* match_ptr = dict_match.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;

    // Unroll by 4
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      uint16_t c0 = codes_ptr[i];
      uint16_t c1 = codes_ptr[i + 1];
      uint16_t c2 = codes_ptr[i + 2];
      uint16_t c3 = codes_ptr[i + 3];

      // Branchless accumulation using multiplication
      local_sum += static_cast<uint64_t>(weights_ptr[i]) * match_ptr[c0];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 1]) * match_ptr[c1];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 2]) * match_ptr[c2];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 3]) * match_ptr[c3];
    }

    // Handle remainder
    for (; i < n; ++i) {
      local_sum += static_cast<uint64_t>(weights_ptr[i]) * match_ptr[codes_ptr[i]];
    }

    sum = local_sum;
  }

  return sum;
}
