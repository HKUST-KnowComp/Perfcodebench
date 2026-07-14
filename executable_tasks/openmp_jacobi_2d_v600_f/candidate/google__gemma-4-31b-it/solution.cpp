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
  
  const size_t c_size = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * c_size;
      const size_t prev_row = row_offset - c_size;
      const size_t next_row = row_offset + c_size;
      
      // Handle first column of the row
      ptr_b[row_offset] = ptr_a[row_offset];
      
      // Inner columns
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_offset + col;
        ptr_b[idx] = (ptr_a[idx] * 4u + 
                      ptr_a[idx - 1] + 
                      ptr_a[idx + 1] + 
                      ptr_a[prev_row + col] + 
                      ptr_a[next_row + col]) / 8u;
      }
      
      // Handle last column of the row
      ptr_b[row_offset + c_size - 1] = ptr_a[row_offset + c_size - 1];
    }
    
    // Boundary rows are constant across steps, but we must ensure they are correct in b
    // Since b was initialized to input and only inner rows are modified, 
    // the first and last rows of b always match the first and last rows of a/input.
    
    std::swap(ptr_a, ptr_b);
  }
  
  // The final result is in ptr_a. We need to ensure the vector returned to checksum is the one ptr_a points to.
  if (ptr_a == b.data()) {
    return checksum_u32(b);
  } else {
    return checksum_u32(a);
  }
}