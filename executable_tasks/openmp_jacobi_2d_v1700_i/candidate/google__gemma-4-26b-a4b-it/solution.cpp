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
  
  const size_t n = static_cast<size_t>(rows) * static_cast<size_t>(cols);
  const size_t c_size = static_cast<size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // We only update the interior. The boundaries remain constant as per the original logic.
    // The original logic: if (row == 0 || col == 0 || row == rows - 1 || col == cols - 1) b[idx] = a[idx];
    // Since boundaries never change, we can initialize b with a and only update the interior.
    
    #pragma omp parallel for schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      const size_t row_offset = static_cast<size_t>(r) * c_size;
      for (int c = 1; c < cols - 1; ++c) {
        const size_t idx = row_offset + c;
        // Using local variables to help compiler with aliasing/optimization
        const uint32_t center = a[idx];
        const uint32_t left   = a[idx - 1];
        const uint32_t right  = a[idx + 1];
        const uint32_t up     = a[idx - c_size];
        const uint32_t down   = a[idx + c_size];
        
        b[idx] = (center * 4u + left + right + up + down) / 8u;
      }
    }
    
    // Note: The original code swaps a and b. 
    // Because we only update the interior of b, and boundaries of a/b are identical to input,
    // we must ensure the boundaries are correctly preserved. 
    // Since b was initialized with input, and we only touch interior, boundaries are correct.
    a.swap(b);
  }

  return checksum_u32(a);
}
