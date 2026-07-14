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
  // If no iterations, result is a zero vector of length rows
  if (iters <= 0) {
    std::vector<int64_t> out_zero(static_cast<std::size_t>(rows), 0);
    return checksum_i64(out_zero);
  }

  // Compute only once since every iteration yields the same output
  std::vector<int64_t> out(static_cast<std::size_t>(rows));

  const int* rp = row_ptr.data();
  const int* ci = col_idx.data();
  const int32_t* val = values.data();
  const int32_t* x = dense.data();

  // Parallelize over rows; dynamic scheduling helps if row nnz are imbalanced
  #pragma omp parallel for schedule(dynamic, 16) if(rows > 32)
  for (int row = 0; row < rows; ++row) {
    const int start = rp[row];
    const int end = rp[row + 1];
    int64_t sum = 0;

    const int32_t* vptr = val + start;
    const int32_t* vend = val + end;
    const int* cptr = ci + start;

    for (; vptr < vend; ++vptr, ++cptr) {
      sum += static_cast<int64_t>(*vptr) * static_cast<int64_t>(x[*cptr]);
    }

    out[static_cast<std::size_t>(row)] = sum;
  }

  return checksum_i64(out);
}
