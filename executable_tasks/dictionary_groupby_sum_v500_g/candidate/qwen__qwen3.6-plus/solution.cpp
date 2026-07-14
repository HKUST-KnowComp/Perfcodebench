#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t hash = 0;
  const size_t n = dictionary.size();
  std::vector<uint64_t> sums(n);
  
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      sums[i] = 0;
    }
    
    const size_t m = codes.size();
    for (size_t i = 0; i < m; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }
    
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash ^= sums[i];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}