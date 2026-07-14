#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  // Precompute which dictionary entries match the prefix condition
  const std::size_t dict_size = dictionary.size();
  std::vector<uint8_t> dict_matches(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
  }
  
  const std::size_t n = codes.size();
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* matches_ptr = dict_matches.data();
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      uint64_t m0 = matches_ptr[codes_ptr[i]];
      uint64_t m1 = matches_ptr[codes_ptr[i + 1]];
      uint64_t m2 = matches_ptr[codes_ptr[i + 2]];
      uint64_t m3 = matches_ptr[codes_ptr[i + 3]];
      
      sum += m0 * weights_ptr[i];
      sum += m1 * weights_ptr[i + 1];
      sum += m2 * weights_ptr[i + 2];
      sum += m3 * weights_ptr[i + 3];
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      if (matches_ptr[codes_ptr[i]]) {
        sum += weights_ptr[i];
      }
    }
  }
  
  return sum;
}
