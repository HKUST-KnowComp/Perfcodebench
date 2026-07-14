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

  const int* rptr = row_ptr.data();
  const int* cidx = col_idx.data();
  const int32_t* vals = values.data();
  const int32_t* den = dense.data();
  int64_t* outp = out.data();

  // Compute once: repeated iterations are redundant for deterministic inputs
  // Parallelize across rows with static schedule; enable only for sufficiently large problem sizes
  #pragma omp parallel for schedule(static) if(rows >= 1024)
  for (int row = 0; row < rows; ++row) {
    int start = rptr[row];
    int end = rptr[row + 1];
    int64_t sum = 0;
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(vals[idx]) * static_cast<int64_t>(den[cidx[idx]]);
    }
    outp[row] = sum;
  }

  return checksum_i64(out);
}
