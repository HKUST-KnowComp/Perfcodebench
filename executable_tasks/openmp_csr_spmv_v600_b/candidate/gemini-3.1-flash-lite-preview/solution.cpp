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
  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      const int start = row_ptr[row];
      const int end = row_ptr[row + 1];
      
      int idx = start;
      // Manual unrolling for performance
      for (; idx + 3 < end; idx += 4) {
        sum += static_cast<int64_t>(values[idx]) * dense[col_idx[idx]];
        sum += static_cast<int64_t>(values[idx + 1]) * dense[col_idx[idx + 1]];
        sum += static_cast<int64_t>(values[idx + 2]) * dense[col_idx[idx + 2]];
        sum += static_cast<int64_t>(values[idx + 3]) * dense[col_idx[idx + 3]];
      }
      for (; idx < end; ++idx) {
        sum += static_cast<int64_t>(values[idx]) * dense[col_idx[idx]];
      }
      out[row] = sum;
    }
  }
  return checksum_i64(out);
}