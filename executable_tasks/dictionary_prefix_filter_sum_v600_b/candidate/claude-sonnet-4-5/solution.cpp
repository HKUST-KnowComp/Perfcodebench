#include "interface.h"

#include <string>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
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
  const std::size_t unroll_size = size - (size % 4);

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    // Unrolled loop for better ILP
    std::size_t i = 0;
    for (; i < unroll_size; i += 4) {
      if (dict_matches[codes[i]]) sum0 += weights[i];
      if (dict_matches[codes[i+1]]) sum1 += weights[i+1];
      if (dict_matches[codes[i+2]]) sum2 += weights[i+2];
      if (dict_matches[codes[i+3]]) sum3 += weights[i+3];
    }
    
    // Handle remainder
    for (; i < size; ++i) {
      if (dict_matches[codes[i]]) {
        sum0 += weights[i];
      }
    }
    
    sum = sum0 + sum1 + sum2 + sum3;
  }
  
  return sum;
}