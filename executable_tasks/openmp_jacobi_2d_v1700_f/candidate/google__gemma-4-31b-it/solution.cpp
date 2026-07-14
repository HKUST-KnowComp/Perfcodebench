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
  if (rows <= 0 || cols <= 0) return 0;
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = input;
  
  uint32_t* ptr_a = a.data();
  uint32_t* ptr_b = b.data();
  
  const size_t stride = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * stride;
      const uint32_t* a_row = &ptr_a[row_offset];
      const uint32_t* a_prev = &ptr_a[row_offset - stride];
      const uint32_t* a_next = &ptr_a[row_offset + stride];
      uint32_t* b_row = &ptr_b[row_offset];

      // Handle first column of the row
      b_row[0] = a_row[0];

      // Core loop: avoid boundary checks
      for (int col = 1; col < cols - 1; ++col) {
        b_row[col] = (a_row[col] * 4u + a_row[col - 1] + a_row[col + 1] + 
                      a_prev[col] + a_next[col]) / 8u;
      }

      // Handle last column of the row
      if (cols > 1) {
        b_row[cols - 1] = a_row[cols - 1];
      }
    }

    // Boundary rows (top and bottom) are already handled by initializing b = input
    // and only iterating row from 1 to rows-2. 
    // However, we must ensure the top and bottom rows of b match a if a changed.
    // Since top/bottom rows of a never change in this specific Jacobi logic,
    // initializing b = input once is sufficient for the boundaries.

    std::swap(ptr_a, ptr_b);
  }

  // The final result is in the buffer pointed to by ptr_a
  // We need to construct a vector or use the pointer to calculate checksum
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* final_data = ptr_a;
  const size_t total_size = static_cast<size_t>(rows) * stride;
  for (size_t i = 0; i < total_size; ++i) {
    hash ^= static_cast<uint64_t>(final_data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}