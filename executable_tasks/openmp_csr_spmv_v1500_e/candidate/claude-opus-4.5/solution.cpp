#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_i64(const int64_t* data, std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;
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
  
  const std::size_t n = static_cast<std::size_t>(rows);
  std::vector<int64_t> out(n);
  
  const int* __restrict rp = row_ptr.data();
  const int* __restrict ci = col_idx.data();
  const int32_t* __restrict vals = values.data();
  const int32_t* __restrict dn = dense.data();
  int64_t* __restrict outp = out.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(dynamic, 256)
    for (int row = 0; row < rows; ++row) {
      const int start = rp[row];
      const int end = rp[row + 1];
      int64_t sum = 0;
      
      int idx = start;
      const int end4 = start + ((end - start) & ~3);
      
      for (; idx < end4; idx += 4) {
        sum += static_cast<int64_t>(vals[idx]) * static_cast<int64_t>(dn[ci[idx]]);
        sum += static_cast<int64_t>(vals[idx + 1]) * static_cast<int64_t>(dn[ci[idx + 1]]);
        sum += static_cast<int64_t>(vals[idx + 2]) * static_cast<int64_t>(dn[ci[idx + 2]]);
        sum += static_cast<int64_t>(vals[idx + 3]) * static_cast<int64_t>(dn[ci[idx + 3]]);
      }
      
      for (; idx < end; ++idx) {
        sum += static_cast<int64_t>(vals[idx]) * static_cast<int64_t>(dn[ci[idx]]);
      }
      
      outp[row] = sum;
    }
  }
  
  return checksum_i64(outp, n);
}