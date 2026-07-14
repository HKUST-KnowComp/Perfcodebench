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
  const size_t c = static_cast<size_t>(cols);
  const size_t r = static_cast<size_t>(rows);

  for (int step = 0; step < steps; ++step) {
    // We only update the interior. The boundaries remain constant from the input.
    // Since the boundary values are copied from 'a' to 'b' in the baseline, 
    // and 'a' starts as 'input', the boundaries are effectively fixed.
    
    #pragma omp parallel for schedule(static)
    for (size_t i = 1; i < r - 1; ++i) {
      const size_t row_offset = i * c;
      for (size_t j = 1; j < c - 1; ++j) {
        const size_t idx = row_offset + j;
        // Jacobi update formula: (4*center + left + right + up + down) / 8
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                  a[idx - c] + a[idx + c]) / 8u;
      }
    }
    
    // Ensure boundaries are preserved if they were modified (though in this logic they aren't)
    // In the baseline, b[idx] = a[idx] for boundaries. 
    // Since b is initialized with input and we only touch interior, we just swap.
    a.swap(b);
  }

  return checksum_u32(a);
}
