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

  if (iters <= 0 || rows <= 0) {
    return checksum_i64(out);
  }

  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* val = values.data();
  const int32_t* x = dense.data();

  // Heuristic: avoid OpenMP overhead for very small problems or single thread
  int max_threads = 1;
  #ifdef _OPENMP
  max_threads = omp_get_max_threads();
  #endif
  bool use_omp = (max_threads > 1) && (rows >= 64);

  if (!use_omp) {
    for (int iter = 0; iter < iters; ++iter) {
      for (int row = 0; row < rows; ++row) {
        int start = rp[row];
        int end = rp[row + 1];
        int64_t sum = 0;
        for (int idx = start; idx < end; ++idx) {
          sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(x[ci[idx]]);
        }
        out[static_cast<std::size_t>(row)] = sum;
      }
    }
  } else {
    #pragma omp parallel
    {
      for (int iter = 0; iter < iters; ++iter) {
        #pragma omp for schedule(guided, 8)
        for (int row = 0; row < rows; ++row) {
          int start = rp[row];
          int end = rp[row + 1];
          int64_t sum = 0;
          for (int idx = start; idx < end; ++idx) {
            sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(x[ci[idx]]);
          }
          out[static_cast<std::size_t>(row)] = sum;
        }
      }
    }
  }

  return checksum_i64(out);
}
