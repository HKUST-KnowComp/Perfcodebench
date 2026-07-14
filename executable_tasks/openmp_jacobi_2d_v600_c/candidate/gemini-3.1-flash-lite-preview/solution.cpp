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
} // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  if (rows <= 2 || cols <= 2) return checksum_u32(input);
  
  std::vector<uint32_t> buffer1 = input;
  std::vector<uint32_t> buffer2(input.size());
  uint32_t* src = buffer1.data();
  uint32_t* dst = buffer2.data();

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      if (row == 0 || row == rows - 1) {
        for (int col = 0; col < cols; ++col) {
          dst[row * cols + col] = src[row * cols + col];
        }
      } else {
        dst[row * cols] = src[row * cols];
        for (int col = 1; col < cols - 1; ++col) {
          size_t idx = (size_t)row * cols + col;
          dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] + 
                      src[idx - cols] + src[idx + cols]) >> 3;
        }
        dst[row * cols + cols - 1] = src[row * cols + cols - 1];
      }
    }
    std::swap(src, dst);
  }

  std::vector<uint32_t> result(src, src + (size_t)rows * cols);
  return checksum_u32(result);
}