#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

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
  if (rows <= 2 || cols <= 2) return checksum_u32(input);

  std::vector<uint32_t> buffer1 = input;
  std::vector<uint32_t> buffer2(input.size());

  uint32_t* src = buffer1.data();
  uint32_t* dst = buffer2.data();

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const uint32_t* row_prev = &src[(row - 1) * cols];
      const uint32_t* row_curr = &src[row * cols];
      const uint32_t* row_next = &src[(row + 1) * cols];
      uint32_t* row_dst = &dst[row * cols];

      row_dst[0] = row_curr[0];
      for (int col = 1; col < cols - 1; ++col) {
        row_dst[col] = (row_curr[col] * 4u + row_curr[col - 1] + row_curr[col + 1] + row_prev[col] + row_next[col]) / 8u;
      }
      row_dst[cols - 1] = row_curr[cols - 1];
    }

    // Copy boundaries
    for (int col = 0; col < cols; ++col) {
      dst[col] = src[col];
      dst[(rows - 1) * cols + col] = src[(rows - 1) * cols + col];
    }

    std::swap(src, dst);
  }

  std::vector<uint32_t> result(src, src + (rows * cols));
  return checksum_u32(result);
}