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
    
  std::vector<int64_t> out(rows, 0);
  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* val = values.data();
  const int32_t* d = dense.data();
  int64_t* o = out.data();

  for (int iter = 0; iter < iters; ++iter) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < rows; ++i) {
      int64_t sum = 0;
      int start = rp[i];
      int end = rp[i+1];
      for (int j = start; j < end; ++j) {
        sum += static_cast<int64_t>(val[j]) * d[ci[j]];
      }
      o[i] = sum;
    }
  }
  
  return checksum_i64(out);
}
