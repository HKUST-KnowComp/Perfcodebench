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
  
  // Precompute which dictionary entries match the prefix rule
  const size_t dict_size = dictionary.size();
  std::vector<uint8_t> dict_matches(dict_size);
  
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    // matches_prefix: s[0] == 'q' && s[1] <= 'm'
    // Need at least 2 chars, but original didn't check - assume valid input
    dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && static_cast<unsigned char>(s[1]) <= 'm') ? 1 : 0;
  }
  
  const size_t n = codes.size();
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* matches_ptr = dict_matches.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
    // Process 4 elements at a time
    size_t i = 0;
    const size_t n4 = n & ~size_t(3);
    
    for (; i < n4; i += 4) {
      uint16_t c0 = codes_ptr[i];
      uint16_t c1 = codes_ptr[i + 1];
      uint16_t c2 = codes_ptr[i + 2];
      uint16_t c3 = codes_ptr[i + 3];
      
      // Use branchless accumulation
      local_sum += static_cast<uint64_t>(weights_ptr[i]) * matches_ptr[c0];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 1]) * matches_ptr[c1];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 2]) * matches_ptr[c2];
      local_sum += static_cast<uint64_t>(weights_ptr[i + 3]) * matches_ptr[c3];
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      uint16_t c = codes_ptr[i];
      local_sum += static_cast<uint64_t>(weights_ptr[i]) * matches_ptr[c];
    }
    
    sum = local_sum;
  }
  
  return sum;
}