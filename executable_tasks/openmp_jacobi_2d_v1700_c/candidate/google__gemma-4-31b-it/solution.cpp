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
  if (rows == 0 || cols == 0) return 0;
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = input;
  
  uint32_t* ptr_a = a.data();
  uint32_t* ptr_b = b.data();
  
  const size_t stride = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * stride;
      const size_t prev_row = row_offset - stride;
      const size_t next_row = row_offset + stride;
      
      // Handle first column of the row separately to keep inner loop clean
      ptr_b[row_offset] = ptr_a[row_offset];
      
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        ptr_b[idx] = (ptr_a[idx] * 4u + 
                      ptr_a[idx - 1] + 
                      ptr_a[idx + 1] + 
                      ptr_a[prev_row + col] + 
                      ptr_a[next_row + col]) / 8u;
      }
      
      // Handle last column of the row
      ptr_b[row_offset + stride - 1] = ptr_a[row_offset + stride - 1];
    }
    
    // Boundary rows (0 and rows-1) are already correct because b was initialized with input
    // and we only update the interior. However, since we swap, we must ensure boundaries 
    // are maintained. Since a and b both start as input, and we only modify interior of b,
    // the boundaries of b always match the boundaries of a.
    
    std::swap(ptr_a, ptr_b);
  }

  // The final result is in the pointer that was 'a' at the end of the last swap
  if (ptr_a != a.data()) {
    a = std::vector<uint32_t>(ptr_a, ptr_a + (rows * cols));
  } else {
    // a is already correct
  }
  
  // To avoid creating a new vector, we can just use the pointer to calculate checksum
  // but the interface requires the checksum of the final grid. 
  // Let's just ensure 'a' contains the final data.
  if (ptr_a != a.data()) {
    std::vector<uint32_t> final_res(ptr_a, ptr_a + (rows * cols));
    return checksum_u32(final_res);
  }
  
  return checksum_u32(a);
}
