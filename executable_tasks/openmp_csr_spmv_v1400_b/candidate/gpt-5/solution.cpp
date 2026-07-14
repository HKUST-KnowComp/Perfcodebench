#include "interface.h"
#include <omp.h>

#include <cstdint>
#include <vector>

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

  if (iters > 0 && rows > 0) {
    const int* rp = row_ptr.data();
    const int* ci = col_idx.data();
    const int32_t* val = values.data();
    const int32_t* x = dense.data();
    const int nrows = rows;

#pragma omp parallel for schedule(guided) if (nrows > 64)
    for (int row = 0; row < nrows; ++row) {
      const int start = rp[row];
      const int end = rp[row + 1];
      int64_t sum = 0;
      for (int idx = start; idx < end; ++idx) {
        sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(x[ci[idx]]);
      }
      out[row] = sum;
    }
  }

  return checksum_i64(out);
}
