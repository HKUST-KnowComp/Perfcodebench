#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t checksum_i64(const std::vector<int64_t>& data) {
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
  // Prepare output initialized to zero to match baseline behavior when iters == 0
  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);

  if (iters <= 0) {
    return checksum_i64(out);
  }

  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* vals = values.data();
  const int32_t* x = dense.data();

  // Compute the SpMV once since repeated iterations would overwrite with the same values
  // Use OpenMP to parallelize across rows.
  #pragma omp parallel for schedule(static) if(rows > 64)
  for (int row = 0; row < rows; ++row) {
    int64_t sum = 0;
    int start = rp[row];
    int end = rp[row + 1];
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(vals[idx]) * static_cast<int64_t>(x[ci[idx]]);
    }
    out[static_cast<std::size_t>(row)] = sum;
  }

  return checksum_i64(out);
}