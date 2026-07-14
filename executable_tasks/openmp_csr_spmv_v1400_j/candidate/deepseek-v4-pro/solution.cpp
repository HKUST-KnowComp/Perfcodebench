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
  const int* row_ptr_data = row_ptr.data();
  const int* col_idx_data = col_idx.data();
  const int32_t* values_data = values.data();
  const int32_t* dense_data = dense.data();
  std::vector<int64_t> out(rows);
  int64_t* out_data = out.data();

  #pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
      #pragma omp for schedule(static)
      for (int row = 0; row < rows; ++row) {
        int64_t sum = 0;
        const int start = row_ptr_data[row];
        const int end = row_ptr_data[row + 1];
        #pragma omp simd
        for (int idx = start; idx < end; ++idx) {
          sum += static_cast<int64_t>(values_data[idx]) *
                 static_cast<int64_t>(dense_data[col_idx_data[idx]]);
        }
        out_data[row] = sum;
      }
      // implicit barrier at the end of the omp for
    }
  }

  return checksum_i64(out);
}
