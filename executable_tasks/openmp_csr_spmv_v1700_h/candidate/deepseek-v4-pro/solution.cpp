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
  if (iters <= 0) {
    std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);
    return checksum_i64(out);
  }

  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);
  const int* __restrict row_ptr_data = row_ptr.data();
  const int* __restrict col_idx_data = col_idx.data();
  const int32_t* __restrict values_data = values.data();
  const int32_t* __restrict dense_data = dense.data();
  int64_t* __restrict out_data = out.data();

  #pragma omp parallel for schedule(static)
  for (int row = 0; row < rows; ++row) {
    int64_t sum = 0;
    const int start = row_ptr_data[row];
    const int end = row_ptr_data[row + 1];
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(values_data[idx]) *
             static_cast<int64_t>(dense_data[col_idx_data[idx]]);
    }
    out_data[row] = sum;
  }

  return checksum_i64(out);
}