#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  const size_t n = codes.size();
  const size_t dict_size = dictionary.size();
  
  uint64_t hash = 0;
  std::vector<uint64_t> sums(dict_size, 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0ULL);
    
    for (size_t i = 0; i < n; ++i) {
      sums[codes[i]] += static_cast<uint64_t>(values[i]);
    }
    
    hash = 1469598103934665603ULL;
    for (size_t k = 0; k < dict_size; ++k) {
      hash = mix(hash, sums[k]);
    }
  }
  return hash;
}