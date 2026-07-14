#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const uint32_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  if (rows <= 0 || cols <= 0) return 0;
  
  size_t size = static_cast<size_t>(rows) * static_cast<size_t>(cols);
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = input;
  
  uint32_t* p_a = a.data();
  uint32_t* p_b = b.data();

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      size_t row_offset = static_cast<size_t>(row) * cols;
      
      if (row == 0 || row == rows - 1) {
        for (int col = 0; col < cols; ++col) {
          p_b[row_offset + col] = p_a[row_offset + col];
        }
      } else {
        // Handle left boundary
        p_b[row_offset] = p_a[row_offset];
        
        // Core computation
        for (int col = 1; col < cols - 1; ++col) {
          size_t idx = row_offset + col;
          p_b[idx] = (p_a[idx] * 4u + p_a[idx - 1] + p_a[idx + 1] +
                      p_a[idx - cols] + p_a[idx + cols]) / 8u;
        }
        
        // Handle right boundary
        p_b[row_offset + cols - 1] = p_a[row_offset + cols - 1];
      }
    }
    std::swap(p_a, p_b);
  }

  return checksum_u32(p_a, size);
}