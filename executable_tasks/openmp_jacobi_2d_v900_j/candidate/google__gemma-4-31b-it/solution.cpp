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
      
      // Process inner columns to avoid boundary checks
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        p_b[idx] = (p_a[idx] * 4u + p_a[idx - 1] + p_a[idx + 1] + 
                    p_a[prev_row + col] + p_a[next_row + col]) / 8u;
      }
    }
    
    // Boundary values remain constant as per original logic (b[idx] = a[idx])
    // Since b was initialized with input and only inner cells are updated,
    // and boundaries of a never change, b's boundaries are already correct.
    
    std::swap(p_a, p_b);
  }

  // The final result is in p_a (after the last swap)
  // We need to ensure the vector returned to checksum_u32 is the one pointed to by p_a
  if (p_a == b.data()) {
    return checksum_u32(b);
  } else {
    return checksum_u32(a);
  }
}