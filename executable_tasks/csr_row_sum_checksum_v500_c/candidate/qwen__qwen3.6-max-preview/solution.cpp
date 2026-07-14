#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  if (iters <= 0) return 0;
  
  uint64_t hash = 1469598103934665603ULL;
  if (row_ptr.size() < 2) return hash;
  
  const uint32_t* rp = row_ptr.data();
  const uint32_t* val = values.data();
  std::size_t num_rows = row_ptr.size() - 1;
  
  for (std::size_t r = 0; r < num_rows; ++r) {
    uint64_t sum = 0;
    uint32_t start = rp[r];
    uint32_t end = rp[r + 1];
    for (uint32_t i = start; i < end; ++i) {
      sum += val[i];
    }
    hash = mix(hash, sum);
  }
  
  return hash;
}