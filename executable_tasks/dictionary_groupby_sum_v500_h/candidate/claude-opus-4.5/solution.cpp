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
  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();
  
  if (n == 0 || dict_size == 0) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dict_size; ++i) {
      hash = mix(hash, 0);
    }
    return hash;
  }
  
  const uint16_t* __restrict codes_ptr = codes.data();
  const uint32_t* __restrict values_ptr = values.data();
  
  std::vector<uint64_t> sums(dict_size);
  uint64_t* __restrict sums_ptr = sums.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(sums_ptr, 0, dict_size * sizeof(uint64_t));
    
    std::size_t i = 0;
    const std::size_t unroll_end = (n >= 4) ? (n - 3) : 0;
    
    for (; i < unroll_end; i += 4) {
      const uint16_t c0 = codes_ptr[i];
      const uint16_t c1 = codes_ptr[i + 1];
      const uint16_t c2 = codes_ptr[i + 2];
      const uint16_t c3 = codes_ptr[i + 3];
      
      sums_ptr[c0] += values_ptr[i];
      sums_ptr[c1] += values_ptr[i + 1];
      sums_ptr[c2] += values_ptr[i + 2];
      sums_ptr[c3] += values_ptr[i + 3];
    }
    
    for (; i < n; ++i) {
      sums_ptr[codes_ptr[i]] += values_ptr[i];
    }
    
    hash = 1469598103934665603ULL;
    for (std::size_t j = 0; j < dict_size; ++j) {
      hash = mix(hash, sums_ptr[j]);
    }
  }
  
  return hash;
}
