#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  const size_t dict_size = dictionary.size();
  const size_t n = codes.size();
  
  if (n == 0 || dict_size == 0) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, 0);
    }
    return hash;
  }
  
  // Use a simple array indexed by dictionary code - much faster than hash map
  std::vector<uint64_t> sums(dict_size);
  
  const uint16_t* __restrict codes_ptr = codes.data();
  const uint32_t* __restrict values_ptr = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear sums
    std::memset(sums.data(), 0, dict_size * sizeof(uint64_t));
    
    // Aggregate values by code index
    // Process in chunks for better cache behavior
    size_t i = 0;
    const size_t unroll = 8;
    const size_t limit = n - (n % unroll);
    
    for (; i < limit; i += unroll) {
      sums[codes_ptr[i]]     += values_ptr[i];
      sums[codes_ptr[i + 1]] += values_ptr[i + 1];
      sums[codes_ptr[i + 2]] += values_ptr[i + 2];
      sums[codes_ptr[i + 3]] += values_ptr[i + 3];
      sums[codes_ptr[i + 4]] += values_ptr[i + 4];
      sums[codes_ptr[i + 5]] += values_ptr[i + 5];
      sums[codes_ptr[i + 6]] += values_ptr[i + 6];
      sums[codes_ptr[i + 7]] += values_ptr[i + 7];
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      sums[codes_ptr[i]] += values_ptr[i];
    }
    
    // Compute checksum in dictionary order (index 0, 1, 2, ...)
    hash = 1469598103934665603ULL;
    for (size_t j = 0; j < dict_size; ++j) {
      hash = mix(hash, sums[j]);
    }
  }
  
  return hash;
}