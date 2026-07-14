#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_i64(const int64_t* data, int size) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
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
  
  // The original code overwrites 'out' in every iteration of 'iters'.
  // Therefore, only the final iteration matters for the checksum.
  // If iters == 0, the output vector remains zeros.
  
  std::vector<int64_t> out(rows, 0);
  if (iters <= 0) return checksum_i64(out.data(), rows);

  const int* r_ptr = row_ptr.data();
  const int* c_idx = col_idx.data();
  const int32_t* vals = values.data();
  const int32_t* d_vec = dense.data();
  int64_t* res = out.data();

  // We only need to perform the SpMV once because the result is overwritten
  // in the original loop: out[row] = sum;
  #pragma omp parallel for
  for (int row = 0; row < rows; ++row) {
    int64_t sum = 0;
    int start = r_ptr[row];
    int end = r_ptr[row + 1];
    for (int idx = start; idx < end; ++idx) {
      sum += static_cast<int64_t>(vals[idx]) * d_vec[c_idx[idx]];
    }
    res[row] = sum;
  }

  return checksum_i64(res, rows);
}