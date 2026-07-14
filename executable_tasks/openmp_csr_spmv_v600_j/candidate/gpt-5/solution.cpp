#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_i64(const std::vector<int64_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const int64_t* p = data.data();
  const std::size_t n = data.size();
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
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

  // If iters == 0, return checksum of zero-initialized vector (matches baseline semantics).
  if (iters > 0 && rows > 0) {
    const int* rowptr = row_ptr.data();
    const int* col = col_idx.data();
    const int32_t* val = values.data();
    const int32_t* den = dense.data();
    int64_t* outp = out.data();

    // Parallelize across rows. Use a threshold to avoid overhead for very small problems.
    #pragma omp parallel for schedule(static) if(rows >= 256)
    for (int row = 0; row < rows; ++row) {
      const int start = rowptr[row];
      const int end = rowptr[row + 1];
      int64_t sum = 0;
      for (int idx = start; idx < end; ++idx) {
        sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(den[col[idx]]);
      }
      outp[row] = sum;
    }
  }

  return checksum_i64(out);
}
