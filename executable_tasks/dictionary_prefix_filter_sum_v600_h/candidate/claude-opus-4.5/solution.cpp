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

  // Precompute which dictionary indices match the prefix rule
  const size_t dict_size = dictionary.size();
  std::vector<uint8_t> match(dict_size, 0);
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    if (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') {
      match[i] = 1;
    }
  }

  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* match_ptr = match.data();
  const size_t n = codes.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t i = 0;
    // Unroll by 4 for better ILP
    for (; i + 4 <= n; i += 4) {
      uint64_t m0 = match_ptr[codes_ptr[i]];
      uint64_t m1 = match_ptr[codes_ptr[i + 1]];
      uint64_t m2 = match_ptr[codes_ptr[i + 2]];
      uint64_t m3 = match_ptr[codes_ptr[i + 3]];
      sum += m0 * weights_ptr[i];
      sum += m1 * weights_ptr[i + 1];
      sum += m2 * weights_ptr[i + 2];
      sum += m3 * weights_ptr[i + 3];
    }
    for (; i < n; ++i) {
      sum += match_ptr[codes_ptr[i]] * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  return sum;
}
