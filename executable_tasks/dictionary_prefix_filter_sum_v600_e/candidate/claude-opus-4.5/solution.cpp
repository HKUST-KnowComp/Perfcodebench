#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {

  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();

  // Precompute which dictionary entries match the prefix
  std::vector<uint8_t> match(dict_size, 0);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    if (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') {
      match[i] = 1;
    }
  }

  const uint16_t* __restrict codes_ptr = codes.data();
  const uint32_t* __restrict weights_ptr = weights.data();
  const uint8_t* __restrict match_ptr = match.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll_limit = n & ~static_cast<std::size_t>(3);

    // Unrolled loop (4x)
    for (; i < unroll_limit; i += 4) {
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