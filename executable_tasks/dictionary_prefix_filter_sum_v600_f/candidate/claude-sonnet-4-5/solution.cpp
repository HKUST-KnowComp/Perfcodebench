#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Pre-compute which dictionary entries match the prefix rule
  std::vector<bool> dict_matches(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
  }
  
  uint64_t sum = 0;
  const std::size_t size = codes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in chunks of 4 for loop unrolling
    std::size_t i = 0;
    const std::size_t limit = size - (size % 4);
    
    for (; i < limit; i += 4) {
      uint64_t local_sum = 0;
      
      if (dict_matches[codes[i]]) {
        local_sum += weights[i];
      }
      if (dict_matches[codes[i + 1]]) {
        local_sum += weights[i + 1];
      }
      if (dict_matches[codes[i + 2]]) {
        local_sum += weights[i + 2];
      }
      if (dict_matches[codes[i + 3]]) {
        local_sum += weights[i + 3];
      }
      
      sum += local_sum;
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      if (dict_matches[codes[i]]) {
        sum += weights[i];
      }
    }
  }
  
  return sum;
}