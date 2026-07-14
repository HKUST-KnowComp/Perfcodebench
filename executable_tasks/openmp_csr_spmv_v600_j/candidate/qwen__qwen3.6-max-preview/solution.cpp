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
  
  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* val = values.data();
  const int32_t* den = dense.data();
  int64_t* out_ptr = out.data();

  #pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
      #pragma omp for schedule(static) nowait
      for (int row = 0; row < rows; ++row) {
        int64_t sum = 0;
        int start = rp[row];
        int end = rp[row + 1];
        for (int idx = start; idx < end; ++idx) {
          sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(den[ci[idx]]);
        }
        out_ptr[row] = sum;
      }
    }
  }
  
  return checksum_i64(out);
}
