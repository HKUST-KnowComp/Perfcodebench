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
  std::vector<uint32_t> buf1 = input;
  std::vector<uint32_t> buf2(size);
  
  uint32_t* a = buf1.data();
  uint32_t* b = buf2.data();

  // Initialize boundaries for b since they are copied from a every step
  // But we can just handle them inside the loop or pre-fill
  
  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      size_t row_offset = static_cast<size_t>(row) * cols;
      uint32_t* a_row = a + row_offset;
      uint32_t* b_row = b + row_offset;

      if (row == 0 || row == rows - 1) {
        for (int col = 0; col < cols; ++col) {
          b_row[col] = a_row[col];
        }
      } else {
        // Boundary columns
        b_row[0] = a_row[0];
        
        const uint32_t* a_prev_row = a + (row_offset - cols);
        const uint32_t* a_next_row = a + (row_offset + cols);
        
        for (int col = 1; col < cols - 1; ++col) {
          b_row[col] = (a_row[col] * 4u + a_row[col - 1] + a_row[col + 1] +
                        a_prev_row[col] + a_next_row[col]) / 8u;
        }
        
        b_row[cols - 1] = a_row[cols - 1];
      }
    }
    std::swap(a, b);
  }

  return checksum_u32(a, size);
}