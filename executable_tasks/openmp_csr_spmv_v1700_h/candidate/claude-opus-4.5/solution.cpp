#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_i64(const int64_t* data, int n) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < n; ++i) {
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
  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);
  
  const int* __restrict__ rp = row_ptr.data();
  const int* __restrict__ ci = col_idx.data();
  const int32_t* __restrict__ vals = values.data();
  const int32_t* __restrict__ dn = dense.data();
  int64_t* __restrict__ outp = out.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      const int start = rp[row];
      const int end = rp[row + 1];
      int64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
      int idx = start;
      const int len = end - start;
      const int unroll_end = start + (len & ~3);
      
      for (; idx < unroll_end; idx += 4) {
        sum0 += static_cast<int64_t>(vals[idx]) * static_cast<int64_t>(dn[ci[idx]]);
        sum1 += static_cast<int64_t>(vals[idx + 1]) * static_cast<int64_t>(dn[ci[idx + 1]]);
        sum2 += static_cast<int64_t>(vals[idx + 2]) * static_cast<int64_t>(dn[ci[idx + 2]]);
        sum3 += static_cast<int64_t>(vals[idx + 3]) * static_cast<int64_t>(dn[ci[idx + 3]]);
      }
      
      for (; idx < end; ++idx) {
        sum0 += static_cast<int64_t>(vals[idx]) * static_cast<int64_t>(dn[ci[idx]]);
      }
      
      outp[row] = sum0 + sum1 + sum2 + sum3;
    }
  }
  return checksum_i64(outp, rows);
}
