#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>
#include <cstddef>

namespace {

uint64_t checksum_i64(const int64_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t csr_spmv_checksum(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int32_t>& values,
    const std::vector<int32_t>& dense,
    int rows,
    int iters) {
  
  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* vals = values.data();
  const int32_t* d = dense.data();
  
  std::vector<int64_t> out(rows, 0);
  int64_t* out_ptr = out.data();

  #pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
      #pragma omp for schedule(guided)
      for (int r = 0; r < rows; ++r) {
        int64_t sum = 0;
        int start = rp[r];
        int end = rp[r+1];
        for (int i = start; i < end; ++i) {
          sum += static_cast<int64_t>(vals[i]) * static_cast<int64_t>(d[ci[i]]);
        }
        out_ptr[r] = sum;
      }
    }
  }

  return checksum_i64(out_ptr, static_cast<size_t>(rows));
}
