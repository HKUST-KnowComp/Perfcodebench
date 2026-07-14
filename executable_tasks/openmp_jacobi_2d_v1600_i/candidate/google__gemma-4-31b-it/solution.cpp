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
      
      const uint32_t* __restrict__ src = p_a;
      uint32_t* __restrict__ dst = p_b;

      // Handle first column of the inner grid
      dst[row_offset] = p_a[row_offset]; // Boundary
      
      // Inner loop: avoid boundary checks
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] + 
                     src[prev_row + col] + src[next_row + col]) / 8u;
      }
      
      // Handle last column of the inner grid
      dst[row_offset + stride - 1] = p_a[row_offset + stride - 1]; // Boundary
    }

    // Boundary rows are already copied since b was initialized with input
    // and we only modify the interior. However, since a and b swap, 
    // we must ensure boundaries are preserved across swaps.
    // The current logic: b is initialized to input. In step 0, interior of b is updated.
    // Then a becomes the updated b. In step 1, interior of b is updated using a.
    // Since boundaries of b were never changed from input, they remain correct.
    
    std::swap(p_a, p_b);
  }

  // The final result is in p_a. We need to put it back into a vector for the checksum.
  // Since we swapped pointers, we need to know which vector p_a points to.
  if (p_a == a.data()) {
    return checksum_u32(a);
  } else {
    return checksum_u32(b);
  }
}