#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t checksum_i64_ptr(const int64_t* data, int len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (int i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL; // FNV-1a 64-bit prime
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
  // Allocate output vector
  std::vector<int64_t> out(static_cast<std::size_t>(rows));

  if (iters <= 0) {
    // Baseline would leave out as all zeros when iters == 0
    for (int i = 0; i < rows; ++i) out[static_cast<std::size_t>(i)] = 0;
    return checksum_i64_ptr(out.data(), rows);
  }

  // Raw pointers for faster access
  const int* __restrict rp = row_ptr.data();
  const int* __restrict ci = col_idx.data();
  const int32_t* __restrict vals = values.data();
  const int32_t* __restrict x = dense.data();
  int64_t* __restrict y = out.data();

  // Parallelize across rows. Use an if-clause to avoid overhead on tiny problems.
  const int nnz = (rows > 0) ? rp[rows] : 0;
  const bool do_parallel = (rows >= 256) || (nnz >= 4096);

  #pragma omp parallel for schedule(static) if(do_parallel)
  for (int row = 0; row < rows; ++row) {
    int start = rp[row];
    int end = rp[row + 1];
    int64_t sum = 0;
    for (int idx = start; idx < end; ++idx) {
      int col = ci[idx];
      sum += static_cast<int64_t>(vals[idx]) * static_cast<int64_t>(x[col]);
    }
    y[row] = sum;
  }

  return checksum_i64_ptr(out.data(), rows);
}
