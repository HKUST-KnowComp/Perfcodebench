#include "interface.h"

#include <cstring>
#include <vector>

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
  uint64_t hash = 0;
  const std::size_t dict_size = dictionary.size();
  const std::size_t data_size = codes.size();
  
  // Pre-allocate aggregation array once
  std::vector<uint64_t> sums(dict_size);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear sums array
    std::memset(sums.data(), 0, dict_size * sizeof(uint64_t));
    
    // Aggregate directly by code index
    for (std::size_t i = 0; i < data_size; ++i) {
      sums[codes[i]] += values[i];
    }
    
    // Compute checksum in dictionary order
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  
  return hash;
}