#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

inline uint64_t checksum_i64(const std::vector<int64_t>& data) {
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
  // Output vector
  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);

  // If iters <= 0, the output remains the zero vector; checksum computed below.
  if (iters > 0 && rows > 0) {
    const int* rp = row_ptr.data();
    const int* ci = col_idx.data();
    const int32_t* val = values.data();
    const int32_t* x = dense.data();
    int64_t* y = out.data();

    // Parallelize across rows. Guided scheduling helps balance irregular row lengths.
    #pragma omp parallel for schedule(guided, 16) if (rows >= 64)
    for (int row = 0; row < rows; ++row) {
      const int start = rp[row];
      const int end = rp[row + 1];
      int64_t sum = 0;

      int idx = start;
      // Unrolled loop for better throughput
      for (; idx + 3 < end; idx += 4) {
        sum += static_cast<int64_t>(val[idx])     * static_cast<int64_t>(x[ci[idx]]);
        sum += static_cast<int64_t>(val[idx + 1]) * static_cast<int64_t>(x[ci[idx + 1]]);
        sum += static_cast<int64_t>(val[idx + 2]) * static_cast<int64_t>(x[ci[idx + 2]]);
        sum += static_cast<int64_t>(val[idx + 3]) * static_cast<int64_t>(x[ci[idx + 3]]);
      }
      for (; idx < end; ++idx) {
        sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(x[ci[idx]]);
      }

      y[row] = sum;
    }
  }

  return checksum_i64(out);
}
