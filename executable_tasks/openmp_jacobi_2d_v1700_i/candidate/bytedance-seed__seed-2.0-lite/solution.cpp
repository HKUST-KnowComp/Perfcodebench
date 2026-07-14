#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  const size_t col_bytes = static_cast<size_t>(cols) * sizeof(uint32_t);
  const size_t cols_st = static_cast<size_t>(cols);

  // Pre-initialize fixed boundaries once (never change, matches original boundary logic)
  if (rows > 0 && cols > 0) {
    // Copy first and last full rows
    memcpy(&b[0], &a[0], col_bytes);
    memcpy(&b[(static_cast<size_t>(rows)-1)*cols_st], &a[(static_cast<size_t>(rows)-1)*cols_st], col_bytes);
    // Copy first and last column for inner rows
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_off = static_cast<size_t>(row) * cols_st;
      b[row_off] = a[row_off];
      b[row_off + cols_st - 1] = a[row_off + cols_st - 1];
    }
  }

  for (int step = 0; step < steps; ++step) {
    // Parallelize inner row computation, static scheduling for perfect load balance
    #pragma omp parallel for schedule(static) default(none) shared(a,b,rows,cols,cols_st)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * cols_st;
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        b[idx] = (a[idx] * 4u + a[idx-1] + a[idx+1] + a[idx - cols_st] + a[idx + cols_st]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}