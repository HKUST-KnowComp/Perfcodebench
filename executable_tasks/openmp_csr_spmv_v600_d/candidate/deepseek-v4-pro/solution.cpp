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
  if (iters > 0) {
    #pragma omp parallel for schedule(dynamic, 64)
    for (int row = 0; row < rows; ++row) {
      int64_t sum = 0;
      const std::size_t start = static_cast<std::size_t>(row_ptr[row]);
      const std::size_t end   = static_cast<std::size_t>(row_ptr[row + 1]);
      for (std::size_t idx = start; idx < end; ++idx) {
        sum += static_cast<int64_t>(values[idx]) *
               static_cast<int64_t>(dense[static_cast<std::size_t>(col_idx[idx])]);
      }
      out[static_cast<std::size_t>(row)] = sum;
    }
  }
  return checksum_i64(out);
}