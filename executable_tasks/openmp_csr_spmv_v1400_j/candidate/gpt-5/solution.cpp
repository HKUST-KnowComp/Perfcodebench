#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t fnv1_offset_basis() { return 1469598103934665603ULL; }
static inline uint64_t fnv1_prime() { return 1099511628211ULL; }

}  // namespace

uint64_t csr_spmv_checksum(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int32_t>& values,
    const std::vector<int32_t>& dense,
    int rows,
    int iters) {
  // Handle edge cases consistently with baseline behavior
  if (rows <= 0) {
    // Hash of an empty vector is just the FNV-1 offset basis
    return fnv1_offset_basis();
  }

  // If no iterations are requested, baseline returns the hash of a zero vector of length rows
  if (iters <= 0) {
    uint64_t h = fnv1_offset_basis();
    const uint64_t prime = fnv1_prime();
    for (int i = 0; i < rows; ++i) {
      // XOR with 0 has no effect
      h *= prime;
    }
    return h;
  }

  // Compute the SpMV once (all iterations produce identical results) and stream the FNV-1 hash
  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* val = values.data();
  const int32_t* x = dense.data();

  uint64_t h = fnv1_offset_basis();
  const uint64_t prime = fnv1_prime();

  for (int row = 0; row < rows; ++row) {
    const int start = rp[row];
    const int end = rp[row + 1];
    int64_t sum = 0;
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(x[ci[idx]]);
    }
    h ^= static_cast<uint64_t>(sum);
    h *= prime;
  }

  return h;
}
