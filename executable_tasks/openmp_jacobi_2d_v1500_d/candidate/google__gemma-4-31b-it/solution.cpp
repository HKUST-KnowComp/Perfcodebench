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
  
  uint32_t* p_a = a.data();
  uint32_t* p_b = b.data();
  
  const size_t stride = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * stride;
      const size_t prev_row = row_offset - stride;
      const size_t next_row = row_offset + stride;
      
      // Process inner columns
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        p_b[idx] = (p_a[idx] * 4u + p_a[idx - 1] + p_a[idx + 1] + 
                    p_a[prev_row + col] + p_a[next_row + col]) / 8u;
      }
      
      // Boundary columns for this row are already handled by initialization
      // since b was initialized as a copy of input and boundaries never change.
    }
    
    // Swap pointers for the next iteration
    uint32_t* temp = p_a;
    p_a = p_b;
    p_b = temp;
  }

  // The final result is in p_a. We need to ensure the vector returned to checksum is the correct one.
  // Since we swapped pointers, we check if p_a points to a or b.
  if (p_a == a.data()) {
    return checksum_u32(a);
  } else {
    return checksum_u32(b);
  }
}