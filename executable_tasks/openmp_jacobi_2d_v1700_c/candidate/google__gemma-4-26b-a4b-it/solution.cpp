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

} // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  if (input.empty()) return 0;
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = input;
  
  const std::size_t n = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  const std::size_t c_size = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // We only need to update the interior. Boundaries are copied/preserved from 'a' to 'b' initially.
    // However, the original logic says b[idx] = a[idx] for boundaries. 
    // Since we swap a and b, and b starts as a copy, we only need to update the interior.
    
    #pragma omp parallel for schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      const std::size_t row_offset = static_cast<std::size_t>(r) * c_size;
      const std::size_t prev_row = row_offset - c_size;
      const std::size_t next_row = row_offset + c_size;
      
      for (std::size_t c = 1; c < c_size - 1; ++c) {
        const std::size_t idx = row_offset + c;
        // b[idx] = (a[idx] * 4 + a[idx-1] + a[idx+1] + a[idx-cols] + a[idx+cols]) / 8
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[prev_row + c] + a[next_row + c]) / 8u;
      }
    }
    
    // The original code handles boundaries by b[idx] = a[idx]. 
    // Since we initialize b = input and only update the interior, 
    // and the boundaries of 'a' never change (they are always assigned a[idx]), 
    // the boundary values remain constant across all steps.
    // We must ensure the boundary values in 'b' are correct if 'a' changed, 
    // but in this specific Jacobi variant, boundaries are fixed.
    
    a.swap(b);
  }

  return checksum_u32(a);
}