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

  // Precompute which dictionary entries match the prefix rule
  std::vector<uint8_t> match(dict_size, 0);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    if (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') {
      match[i] = 1;
    }
  }

  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* match_ptr = match.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll = 8;
    const std::size_t limit = n - (n % unroll);

    for (; i < limit; i += unroll) {
      local_sum += static_cast<uint64_t>(weights_ptr[i])     * match_ptr[codes_ptr[i]];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 1]) * match_ptr[codes_ptr[i + 1]];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 2]) * match_ptr[codes_ptr[i + 2]];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 3]) * match_ptr[codes_ptr[i + 3]];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 4]) * match_ptr[codes_ptr[i + 4]];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 5]) * match_ptr[codes_ptr[i + 5]];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 6]) * match_ptr[codes_ptr[i + 6]];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 7]) * match_ptr[codes_ptr[i + 7]];
    }

    for (; i < n; ++i) {
      local_sum += static_cast<uint64_t>(weights_ptr[i]) * match_ptr[codes_ptr[i]];
    }

    sum = local_sum;
  }

  return sum;
}
