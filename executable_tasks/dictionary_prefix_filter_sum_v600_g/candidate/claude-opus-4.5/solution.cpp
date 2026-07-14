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

  // Precompute which dictionary entries match the prefix rule: s[0]=='q' && s[1]<='m'
  std::vector<uint8_t> dict_match(dict_size);
  for (std::size_t d = 0; d < dict_size; ++d) {
    const std::string& s = dictionary[d];
    // Assuming strings have at least 2 chars based on original code's unchecked access
    dict_match[d] = (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') ? 1 : 0;
  }

  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* match_ptr = dict_match.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;

    // Process 4 elements at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      uint64_t s0 = match_ptr[codes_ptr[i]]     ? weights_ptr[i]     : 0;
      uint64_t s1 = match_ptr[codes_ptr[i + 1]] ? weights_ptr[i + 1] : 0;
      uint64_t s2 = match_ptr[codes_ptr[i + 2]] ? weights_ptr[i + 2] : 0;
      uint64_t s3 = match_ptr[codes_ptr[i + 3]] ? weights_ptr[i + 3] : 0;
      sum += s0 + s1 + s2 + s3;
    }

    // Handle remainder
    for (; i < n; ++i) {
      if (match_ptr[codes_ptr[i]]) {
        sum += weights_ptr[i];
      }
    }
  }

  return sum;
}