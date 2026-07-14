#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {

  // Precompute which dictionary entries match the prefix rule
  const size_t dict_size = dictionary.size();
  std::vector<uint8_t> dict_match(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    // matches_prefix: s[0] == 'q' && s[1] <= 'm'
    // Assuming strings have at least 2 chars based on original code
    dict_match[i] = (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') ? 1 : 0;
  }

  const size_t n = codes.size();
  const uint16_t* __restrict codes_ptr = codes.data();
  const uint32_t* __restrict weights_ptr = weights.data();
  const uint8_t* __restrict match_ptr = dict_match.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    size_t i = 0;

    // Process 8 elements at a time
    const size_t unroll = 8;
    const size_t limit = n - (n % unroll);

    for (; i < limit; i += unroll) {
      local_sum += match_ptr[codes_ptr[i]]     * static_cast<uint64_t>(weights_ptr[i]);
      local_sum += match_ptr[codes_ptr[i + 1]] * static_cast<uint64_t>(weights_ptr[i + 1]);
      local_sum += match_ptr[codes_ptr[i + 2]] * static_cast<uint64_t>(weights_ptr[i + 2]);
      local_sum += match_ptr[codes_ptr[i + 3]] * static_cast<uint64_t>(weights_ptr[i + 3]);
      local_sum += match_ptr[codes_ptr[i + 4]] * static_cast<uint64_t>(weights_ptr[i + 4]);
      local_sum += match_ptr[codes_ptr[i + 5]] * static_cast<uint64_t>(weights_ptr[i + 5]);
      local_sum += match_ptr[codes_ptr[i + 6]] * static_cast<uint64_t>(weights_ptr[i + 6]);
      local_sum += match_ptr[codes_ptr[i + 7]] * static_cast<uint64_t>(weights_ptr[i + 7]);
    }

    // Handle remainder
    for (; i < n; ++i) {
      local_sum += match_ptr[codes_ptr[i]] * static_cast<uint64_t>(weights_ptr[i]);
    }

    sum = local_sum;
  }

  return sum;
}
