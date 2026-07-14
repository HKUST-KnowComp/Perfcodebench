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
  std::vector<uint32_t> b(a.size(), 0);
  
  const std::size_t total_size = static_cast<std::size_t>(rows) * cols;
  const std::size_t c_size = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // Handle boundaries first (they remain constant or follow specific rules)
    // In this specific Jacobi implementation, boundaries are copied from 'a' to 'b'
    // We can optimize by only updating the interior and then handling edges.
    
    #pragma omp parallel
    {
      // 1. Copy boundaries to b
      #pragma omp for nowait
      for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
          if (r == 0 || c == 0 || r == rows - 1 || c == cols - 1) {
            std::size_t idx = static_cast<std::size_t>(r) * c_size + c;
            b[idx] = a[idx];
          }
        }
      }

      // 2. Compute interior
      // We use a collapsed loop to increase the iteration space for OpenMP
      #pragma omp for collapse(2)
      for (int r = 1; r < rows - 1; ++r) {
        for (int c = 1; c < cols - 1; ++c) {
          const std::size_t idx = static_cast<std::size_t>(r) * c_size + c;
          // Using local variables to help compiler with aliasing/optimization
          const uint32_t center = a[idx];
          const uint32_t left   = a[idx - 1];
          const uint32_t right  = a[idx + 1];
          const uint32_t up     = a[idx - c_size];
          const uint32_t down   = a[idx + c_size];
          
          b[idx] = (center * 4u + left + right + up + down) / 8u;
        }
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}
