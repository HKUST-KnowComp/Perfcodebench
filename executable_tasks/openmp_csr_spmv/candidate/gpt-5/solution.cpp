#include "interface.h"

#include <cstdint>
#include <vector>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace {

inline uint64_t checksum_i64(const std::vector<int64_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const int64_t* p = data.data();
  const int64_t* e = p + data.size();
  for (; p != e; ++p) {
    hash ^= static_cast<uint64_t>(*p);
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

  if (iters > 0 && rows > 0) {
    const int* __restrict rp = row_ptr.data();
    const int* __restrict ci = col_idx.data();
    const int32_t* __restrict val = values.data();
    const int32_t* __restrict d = dense.data();
    int64_t* __restrict o = out.data();

    bool do_parallel = false;
#ifdef _OPENMP
    do_parallel = (rows >= 256) && (omp_get_max_threads() > 1);
#endif

    if (do_parallel) {
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
      for (int row = 0; row < rows; ++row) {
        const int start = rp[row];
        const int end = rp[row + 1];
        int64_t sum = 0;
        for (int idx = start; idx < end; ++idx) {
          sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(d[ci[idx]]);
        }
        o[row] = sum;
      }
    } else {
      for (int row = 0; row < rows; ++row) {
        const int start = rp[row];
        const int end = rp[row + 1];
        int64_t sum = 0;
        for (int idx = start; idx < end; ++idx) {
          sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(d[ci[idx]]);
        }
        o[row] = sum;
      }
    }
  }

  return checksum_i64(out);
}
