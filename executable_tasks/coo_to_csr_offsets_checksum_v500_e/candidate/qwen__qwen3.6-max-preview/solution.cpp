#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(offsets.data(), 0, offsets.size() * sizeof(uint32_t));
    
    for (uint32_t r : row_ids) {
      offsets[static_cast<std::size_t>(r) + 1u]++;
    }
    
    uint32_t acc = 0;
    for (std::size_t i = 0; i < offsets.size(); ++i) {
      acc += offsets[i];
      offsets[i] = acc;
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}