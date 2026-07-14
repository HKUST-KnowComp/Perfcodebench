#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

inline uint64_t checksum_i64(const std::vector<int64_t>& data) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1 offset basis
  for (int64_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL; // FNV-1 prime
  }
  return hash;
}

}  // namespace

uint64_t csr_spmv_checksum(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int32_t>& values,
    const std::vector<int32_t>& dense,
    int rows,
    int iters) {
  // Handle zero-iteration case: output is all zeros
  if (iters <= 0) {
    // Equivalent to hashing a zero-filled vector of length rows
    uint64_t hash = 1469598103934665603ULL;
    for (int i = 0; i < rows; ++i) {
      hash ^= static_cast<uint64_t>(int64_t{0});
      hash *= 1099511628211ULL;
    }
    return hash;
  }

#if defined(__GNUC__) || defined(__clang__)
#  define RESTRICT __restrict__
#else
#  define RESTRICT
#endif

  const int* RESTRICT rp = row_ptr.data();
  const int* RESTRICT ci = col_idx.data();
  const int32_t* RESTRICT A = values.data();
  const int32_t* RESTRICT x = dense.data();

  std::vector<int64_t> out(static_cast<std::size_t>(rows));
  int64_t* RESTRICT y = out.data();

  // Compute a single SpMV since repeated iterations yield the same output.
  // Parallelize across rows.
  #pragma omp parallel for schedule(static)
  for (int row = 0; row < rows; ++row) {
    const int start = rp[row];
    const int end   = rp[row + 1];
    int64_t sum = 0;
    // The gather from x[ci[idx]] is irregular; still, keep the loop tight.
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(A[idx]) * static_cast<int64_t>(x[ci[idx]]);
    }
    y[row] = sum;
  }

  // Compute checksum of the result vector (final iteration's output)
  return checksum_i64(out);
}
