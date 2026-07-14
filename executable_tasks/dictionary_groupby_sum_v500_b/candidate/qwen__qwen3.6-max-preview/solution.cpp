#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <string>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  
  uint64_t hash = 0;
  size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);
  
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* values_ptr = values.data();
  size_t n = codes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);
    
    for (size_t i = 0; i < n; ++i) {
      sums[codes_ptr[i]] += values_ptr[i];
    }
    
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash ^= sums[i];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}