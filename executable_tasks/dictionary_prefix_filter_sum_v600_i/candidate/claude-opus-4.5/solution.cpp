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
  
  const size_t dict_size = dictionary.size();
  const size_t n = codes.size();
  
  if (n == 0 || iters <= 0) {
    return 0;
  }
  
  // Precompute which dictionary entries match the prefix condition
  // matches_prefix: s[0] == 'q' && s[1] <= 'm'
  std::vector<uint8_t> dict_matches(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    // Need at least 2 chars, s[0]=='q', s[1]<='m'
    dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') ? 1 : 0;
  }
  
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* matches_ptr = dict_matches.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    
    size_t i = 0;
    const size_t unroll = 8;
    const size_t limit = n - (n % unroll);
    
    for (; i < limit; i += unroll) {
      // Unrolled loop with multiple accumulators
      if (matches_ptr[codes_ptr[i]]) acc0 += weights_ptr[i];
      if (matches_ptr[codes_ptr[i+1]]) acc1 += weights_ptr[i+1];
      if (matches_ptr[codes_ptr[i+2]]) acc2 += weights_ptr[i+2];
      if (matches_ptr[codes_ptr[i+3]]) acc3 += weights_ptr[i+3];
      if (matches_ptr[codes_ptr[i+4]]) acc0 += weights_ptr[i+4];
      if (matches_ptr[codes_ptr[i+5]]) acc1 += weights_ptr[i+5];
      if (matches_ptr[codes_ptr[i+6]]) acc2 += weights_ptr[i+6];
      if (matches_ptr[codes_ptr[i+7]]) acc3 += weights_ptr[i+7];
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      if (matches_ptr[codes_ptr[i]]) {
        acc0 += weights_ptr[i];
      }
    }
    
    sum = acc0 + acc1 + acc2 + acc3;
  }
  
  return sum;
}