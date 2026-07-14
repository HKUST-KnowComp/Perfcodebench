#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

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
  size_t n = codes.size();
  size_t dict_size = dictionary.size();
  std::vector<uint64_t> sums(dict_size, 0);
  
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* values_ptr = values.data();
  uint64_t* sums_ptr = sums.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums_ptr, sums_ptr + dict_size, 0);
    
    for (size_t i = 0; i < n; ++i) {
      sums_ptr[codes_ptr[i]] += values_ptr[i];
    }
    
    hash = 1469598103934665603ULL;
    for (size_t j = 0; j < dict_size; ++j) {
      hash = mix(hash, sums_ptr[j]);
    }
  }
  return hash;
}
