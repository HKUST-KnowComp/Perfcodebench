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
  
  const int* r_ptr = row_ptr.data();
  const int* c_idx = col_idx.data();
  const int32_t* vals = values.data();
  const int32_t* d = dense.data();
  int64_t* o = out.data();

  #pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
      #pragma omp for schedule(static)
      for (int row = 0; row < rows; ++row) {
        int64_t sum0 = 0;
        int64_t sum1 = 0;
        int64_t sum2 = 0;
        int64_t sum3 = 0;
        
        int start = r_ptr[row];
        int end = r_ptr[row + 1];
        int idx = start;
        
        for (; idx <= end - 4; idx += 4) {
          sum0 += static_cast<int64_t>(vals[idx]) * d[c_idx[idx]];
          sum1 += static_cast<int64_t>(vals[idx + 1]) * d[c_idx[idx + 1]];
          sum2 += static_cast<int64_t>(vals[idx + 2]) * d[c_idx[idx + 2]];
          sum3 += static_cast<int64_t>(vals[idx + 3]) * d[c_idx[idx + 3]];
        }
        
        for (; idx < end; ++idx) {
          sum0 += static_cast<int64_t>(vals[idx]) * d[c_idx[idx]];
        }
        
        o[row] = sum0 + sum1 + sum2 + sum3;
      }
    }
  }
  
  return checksum_i64(out);
}
