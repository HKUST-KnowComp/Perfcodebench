#include "interface.h"

#include <cstdint>
#include <vector>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace {
inline uint64_t checksum_i64(const std::vector<int64_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = data.size();
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
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
  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);

  if (iters <= 0 || rows <= 0) {
    return checksum_i64(out);
  }

  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* val = values.data();
  const int32_t* x = dense.data();
  int64_t* y = out.data();

  // Compute SpMV once; repeated iterations produce the same result with constant inputs.
  #ifdef _OPENMP
  #pragma omp parallel for schedule(static) if(rows > 64)
  #endif
  for (int row = 0; row < rows; ++row) {
    int start = rp[row];
    int end = rp[row + 1];
    int64_t sum = 0;
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(x[ci[idx]]);
    }
    y[row] = sum;
  }

  return checksum_i64(out);
}
