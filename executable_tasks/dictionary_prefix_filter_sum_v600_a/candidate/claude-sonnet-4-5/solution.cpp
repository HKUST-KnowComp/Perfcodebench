#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline bool matches_prefix(const std::string& s) {
  return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Pre-compute which dictionary entries match the prefix
  std::vector<bool> dict_matches(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    dict_matches[i] = matches_prefix(dictionary[i]);
  }

  uint64_t sum = 0;
  const std::size_t size = codes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    const std::size_t unroll_limit = size - (size % 4);
    for (; i < unroll_limit; i += 4) {
      if (dict_matches[codes[i]]) s0 += weights[i];
      if (dict_matches[codes[i+1]]) s1 += weights[i+1];
      if (dict_matches[codes[i+2]]) s2 += weights[i+2];
      if (dict_matches[codes[i+3]]) s3 += weights[i+3];
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      if (dict_matches[codes[i]]) {
        s0 += weights[i];
      }
    }
    
    sum = s0 + s1 + s2 + s3;
  }
  
  return sum;
}