#include "interface.h"

#include <vector>
#include <algorithm>
#include <cstdint>

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
  
  std::vector<uint64_t> sums(dict_size, 0);
  uint64_t hash = 0;
  
  const uint16_t* c_ptr = codes.data();
  const uint32_t* v_ptr = values.data();
  uint64_t* s_ptr = sums.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(s_ptr, s_ptr + dict_size, 0);
    
    for (std::size_t i = 0; i < n; ++i) {
      s_ptr[c_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
    }
    
    hash = 1469598103934665603ULL;
    for (std::size_t j = 0; j < dict_size; ++j) {
      hash = mix(hash, s_ptr[j]);
    }
  }
  
  return hash;
}
