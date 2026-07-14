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
  
  const int* __restrict p_row_ptr = row_ptr.data();
  const int* __restrict p_col_idx = col_idx.data();
  const int32_t* __restrict p_values = values.data();
  const int32_t* __restrict p_dense = dense.data();
  int64_t* __restrict p_out = out.data();

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      int start = p_row_ptr[row];
      int end = p_row_ptr[row + 1];
      
      int idx = start;
      // Manual unrolling for performance
      for (; idx <= end - 4; idx += 4) {
        sum += static_cast<int64_t>(p_values[idx]) * p_dense[p_col_idx[idx]];
        sum += static_cast<int64_t>(p_values[idx + 1]) * p_dense[p_col_idx[idx + 1]];
        sum += static_cast<int64_t>(p_values[idx + 2]) * p_dense[p_col_idx[idx + 2]];
        sum += static_cast<int64_t>(p_values[idx + 3]) * p_dense[p_col_idx[idx + 3]];
      }
      for (; idx < end; ++idx) {
        sum += static_cast<int64_t>(p_values[idx]) * p_dense[p_col_idx[idx]];
      }
      p_out[row] = sum;
    }
  }
  return checksum_i64(out);
}