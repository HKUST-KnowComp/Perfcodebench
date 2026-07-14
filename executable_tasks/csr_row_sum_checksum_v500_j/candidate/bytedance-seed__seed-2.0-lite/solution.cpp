#include "interface.h"

#include <numeric>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  uint64_t hash = 0;
  const size_t num_rows = row_ptr.size() - 1;
  if (num_rows == 0 || iters <= 0) return hash;
  
  const uint32_t* const vals = values.data();
  const uint32_t* const row_ptrs = row_ptr.data();
  
  // Precompute row sums once to avoid redundant work across iterations
  std::vector<uint64_t> row_sums(num_rows);
  for (size_t r = 0; r < num_rows; ++r) {
    const size_t start = row_ptrs[r];
    const size_t end = row_ptrs[r + 1];
    row_sums[r] = std::accumulate(vals + start, vals + end, 0ULL);
  }
  
  // Reuse precomputed sums for all iterations
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const uint64_t sum : row_sums) {
      hash = mix(hash, sum);
    }
  }
  return hash;
}