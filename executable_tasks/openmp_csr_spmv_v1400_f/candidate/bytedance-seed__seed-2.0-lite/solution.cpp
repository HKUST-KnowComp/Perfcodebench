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
  
  // Extract raw pointers for fast inner loop access
  const int* row_ptr_p = row_ptr.data();
  const int* col_idx_p = col_idx.data();
  const int32_t* values_p = values.data();
  const int32_t* dense_p = dense.data();
  int64_t* out_p = out.data();

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(static) default(none) \
    shared(rows, row_ptr_p, col_idx_p, values_p, dense_p, out_p) \
    private(row, sum, idx)
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      const int start = row_ptr_p[row];
      const int end = row_ptr_p[row + 1];
      for (int idx = start; idx < end; ++idx) {
        sum += static_cast<int64_t>(values_p[idx]) * static_cast<int64_t>(dense_p[col_idx_p[idx]]);
      }
      out_p[row] = sum;
    }
  }
  return checksum_i64(out);
}