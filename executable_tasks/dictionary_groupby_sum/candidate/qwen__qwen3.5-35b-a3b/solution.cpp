#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t hash = 0;
  const size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset sums
    std::fill(sums.begin(), sums.end(), 0);
    
    // Aggregate values by code index
    for (size_t i = 0; i < codes.size(); ++i) {
      sums[codes[i]] += values[i];
    }
    
    // Compute checksum in dictionary order
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash ^= sums[i];
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}