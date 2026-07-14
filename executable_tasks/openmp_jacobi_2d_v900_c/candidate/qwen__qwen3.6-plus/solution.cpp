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
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  
  const size_t last_row_idx = static_cast<size_t>(rows - 1) * cols;
  
  #pragma omp parallel
  {
    for (int step = 0; step < steps; ++step) {
      // Copy boundaries sequentially (fast O(rows+cols))
      for (int col = 0; col < cols; ++col) {
        b[col] = a[col];
        b[last_row_idx + col] = a[last_row_idx + col];
      }
      for (int row = 1; row < rows - 1; ++row) {
        size_t r_idx = static_cast<size_t>(row) * cols;
        b[r_idx] = a[r_idx];
        b[r_idx + cols - 1] = a[r_idx + cols - 1];
      }
      
      // Parallelize interior computation
      #pragma omp for schedule(static)
      for (int row = 1; row < rows - 1; ++row) {
        size_t row_start = static_cast<size_t>(row) * cols;
        for (int col = 1; col < cols - 1; ++col) {
          size_t idx = row_start + col;
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - cols] + a[idx + cols]) >> 3;
        }
      }
      
      // Synchronize and swap buffers
      #pragma omp single
      a.swap(b);
    }
  }
  return checksum_u32(a);
}
