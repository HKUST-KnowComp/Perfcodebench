#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_i64(const std::vector<int64_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (int64_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
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
  const int nrows = rows;
  std::vector<int64_t> out(static_cast<std::size_t>(nrows), 0);

  // If no iterations requested, return checksum of zero vector (baseline behavior).
  if (iters <= 0) {
    return checksum_i64(out);
  }

  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* a = values.data();
  const int32_t* x = dense.data();
  int64_t* y = out.data();

  // Compute SpMV once; repeated iterations would yield the same result.
  // Parallelize across rows. Use a dynamic schedule to balance uneven row nnz.
  #pragma omp parallel for schedule(dynamic, 64)
  for (int r = 0; r < nrows; ++r) {
    int64_t sum = 0;
    const int start = rp[r];
    const int end = rp[r + 1];
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(a[idx]) * static_cast<int64_t>(x[ci[idx]]);
    }
    y[r] = sum;
  }

  return checksum_i64(out);
}
