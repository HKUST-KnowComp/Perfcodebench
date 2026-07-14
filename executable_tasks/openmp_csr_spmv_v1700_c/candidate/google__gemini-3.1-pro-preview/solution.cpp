#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_i64(const int64_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
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
  
  if (rows <= 0) return 1469598103934665603ULL;

  std::vector<int64_t> out(rows);
  int64_t* __restrict__ out_ptr = out.data();
  const int* __restrict__ r_ptr = row_ptr.data();
  const int* __restrict__ c_idx = col_idx.data();
  const int32_t* __restrict__ val_ptr = values.data();
  const int32_t* __restrict__ d_ptr = dense.data();

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(guided)
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      int start = r_ptr[row];
      int end = r_ptr[row + 1];
      
      // Manual unrolling or compiler auto-vectorization target
      #pragma omp simd reduction(+:sum)
      for (int idx = start; idx < end; ++idx) {
        sum += static_cast<int64_t>(val_ptr[idx]) * static_cast<int64_t>(d_ptr[c_idx[idx]]);
      }
      out_ptr[row] = sum;
    }
  }

  return checksum_i64(out_ptr, static_cast<size_t>(rows));
}