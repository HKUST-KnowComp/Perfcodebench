#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>

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
  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);
  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for default(none) shared(row_ptr, col_idx, values, dense, out, rows)
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      const std::size_t row_idx = static_cast<std::size_t>(row);
      const int start = row_ptr[row_idx];
      const int end = row_ptr[row_idx + 1];
      #pragma omp simd reduction(+:sum)
      for (int idx = start; idx < end; ++idx) {
        const std::size_t idx_sz = static_cast<std::size_t>(idx);
        sum += static_cast<int64_t>(values[idx_sz]) * static_cast<int64_t>(dense[static_cast<std::size_t>(col_idx[idx_sz])]);
      }
      out[row_idx] = sum;
    }
  }
  return checksum_i64(out);
}