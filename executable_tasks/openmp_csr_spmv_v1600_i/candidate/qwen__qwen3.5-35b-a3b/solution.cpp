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

  const int* row_ptr_data = row_ptr.data();
  const int* col_idx_data = col_idx.data();
  const int32_t* values_data = values.data();
  const int32_t* dense_data = dense.data();

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      int start = row_ptr_data[row];
      int end = row_ptr_data[row + 1];
      #pragma omp simd
      for (int idx = start; idx < end; ++idx) {
        sum += static_cast<int64_t>(values_data[idx]) *
               static_cast<int64_t>(dense_data[col_idx_data[idx]]);
      }
      out[static_cast<std::size_t>(row)] = sum;
    }
  }
  return checksum_i64(out);
}