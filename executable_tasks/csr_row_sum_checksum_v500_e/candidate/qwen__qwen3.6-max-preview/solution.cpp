#include "interface.h"
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  if (row_ptr.size() < 2) return 0;
  
  size_t num_rows = row_ptr.size() - 1;
  std::vector<uint64_t> row_sums(num_rows);
  const uint32_t* val_data = values.data();
  const uint32_t* ptr_data = row_ptr.data();
  
  for (size_t r = 0; r < num_rows; ++r) {
    uint64_t sum = 0;
    const uint32_t* p = val_data + ptr_data[r];
    const uint32_t* end = val_data + ptr_data[r + 1];
    while (p < end) {
      sum += *p++;
    }
    row_sums[r] = sum;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint64_t s : row_sums) {
      hash = mix(hash, s);
    }
  }
  return hash;
}