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
  uint64_t hash = 0;
  const size_t n = dictionary.size();
  std::vector<uint64_t> sums(n);
  
  const size_t m = codes.size();
  const uint16_t* c_ptr = codes.data();
  const uint32_t* v_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      sums[i] = 0;
    }
    
    for (size_t i = 0; i < m; ++i) {
      sums[c_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
    }
    
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      hash = mix(hash, sums[i]);
    }
  }
  return hash;
}
