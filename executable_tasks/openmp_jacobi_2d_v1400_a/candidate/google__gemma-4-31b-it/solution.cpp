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
      const uint32_t* __restrict__ src = p_a + row_offset;
      uint32_t* __restrict__ dst = p_b + row_offset;
      
      const uint32_t* __restrict__ prev_row = p_a + (row_offset - stride);
      const uint32_t* __restrict__ next_row = p_a + (row_offset + stride);

      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        dst[col] = (src[col] * 4u + src[col - 1] + src[col + 1] + 
                    prev_row[col] + next_row[col]) / 8u;
      }
    }
    
    // Boundary values remain constant as per original logic (b[idx] = a[idx])
    // Since b was initialized with input and only inner cells are updated,
    // and a.swap(b) happens, we must ensure boundaries are preserved.
    // The original code does b[idx] = a[idx] for boundaries every step.
    // Since boundaries never change, we only need to ensure they are correct once.
    
    uint32_t* temp = p_a;
    p_a = p_b;
    p_b = temp;
  }

  // The final result is in p_a
  if (p_a != a.data()) {
    a = b;
  } else {
    // a is already correct
  }
  
  return checksum_u32(a);
}