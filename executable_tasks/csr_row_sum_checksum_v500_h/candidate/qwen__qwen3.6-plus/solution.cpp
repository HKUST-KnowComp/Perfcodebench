#include "interface.h"
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* val_data = values.data();
  const uint32_t* rp_data = row_ptr.data();
  size_t num_rows = row_ptr.size();

  for (size_t r = 0; r + 1 < num_rows; ++r) {
    uint64_t sum = 0;
    const uint32_t* p = val_data + rp_data[r];
    const uint32_t* e = val_data + rp_data[r + 1];
    while (p != e) {
      sum += *p++;
    }
    hash ^= sum;
    hash *= 1099511628211ULL;
  }
  return hash;
}