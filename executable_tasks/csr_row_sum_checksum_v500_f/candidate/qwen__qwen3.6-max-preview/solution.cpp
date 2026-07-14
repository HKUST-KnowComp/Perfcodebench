#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  if (iters <= 0) return hash;

  size_t num_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
  if (num_rows == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  std::vector<uint64_t> row_sums(num_rows);
  const uint32_t* rp = row_ptr.data();
  const uint32_t* val = values.data();

  for (size_t r = 0; r < num_rows; ++r) {
    uint64_t sum = 0;
    const uint32_t* p = val + rp[r];
    const uint32_t* e = val + rp[r + 1];
    while (p < e) {
      sum += *p++;
    }
    row_sums[r] = sum;
  }

  const uint64_t* sums = row_sums.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_rows; ++r) {
      hash = mix(hash, sums[r]);
    }
  }

  return hash;
}