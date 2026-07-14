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
  
  const int* __restrict rp = row_ptr.data();
  const int* __restrict ci = col_idx.data();
  const int32_t* __restrict val = values.data();
  const int32_t* __restrict den = dense.data();
  int64_t* __restrict out_ptr = out.data();

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      int start = rp[row];
      int end = rp[row + 1];
      
      // Manual unrolling for better pipeline utilization
      int idx = start;
      for (; idx <= end - 4; idx += 4) {
        sum += static_cast<int64_t>(val[idx]) * den[ci[idx]];
        sum += static_cast<int64_t>(val[idx + 1]) * den[ci[idx + 1]];
        sum += static_cast<int64_t>(val[idx + 2]) * den[ci[idx + 2]];
        sum += static_cast<int64_t>(val[idx + 3]) * den[ci[idx + 3]];
      }
      for (; idx < end; ++idx) {
        sum += static_cast<int64_t>(val[idx]) * den[ci[idx]];
      }
      out_ptr[row] = sum;
    }
  }
  return checksum_i64(out);
}