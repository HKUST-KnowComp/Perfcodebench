#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t final_hash = 0;
  const uint32_t* r_ptr = row_ptr.data();
  const uint32_t* v_ptr = values.data();
  const size_t num_rows = row_ptr.size() - 1;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_rows; ++r) {
      uint32_t start = r_ptr[r];
      uint32_t end = r_ptr[r + 1];
      uint64_t sum = 0;
      
      // Direct pointer access to avoid vector allocation and slicing
      for (uint32_t i = start; i < end; ++i) {
        sum += v_ptr[i];
      }
      hash = mix(hash, sum);
    }
    final_hash = hash;
  }
  return final_hash;
}