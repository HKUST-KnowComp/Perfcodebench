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
  if (rows <= 0 || cols <= 0) return 0;
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  
  // Pre-copy boundaries to b to avoid conditional checks in the hot loop
  // However, since boundaries are constant across steps, we can just ensure 
  // they are copied once or handled correctly.
  // For simplicity and speed, we handle the interior in parallel and edges separately.

  const std::size_t total_size = static_cast<std::size_t>(rows) * cols;
  const std::size_t c_size = static_cast<std::size_t>(cols);

  for (int step = 0; step < steps; ++step) {
    // 1. Handle boundaries (Rows 0 and rows-1, and Cols 0 and cols-1)
    // These values remain constant throughout the iterations because they are assigned a[idx]
    // But we must ensure they are correctly set in 'b' if 'a' changes (though they don't).
    // Actually, the rule says b[idx] = a[idx] for boundaries. 
    // Since boundaries never change, we can initialize b with a's boundaries once.
    if (step == 0) {
        for (int r = 0; r < rows; ++r) {
            b[static_cast<std::size_t>(r) * c_size] = a[static_cast<std::size_t>(r) * c_size];
            b[static_cast<std::size_t>(r) * c_size + c_size - 1] = a[static_cast<std::size_t>(r) * c_size + c_size - 1];
        }
        for (int c = 0; c < cols; ++c) {
            b[c] = a[c];
            b[static_cast<std::size_t>(rows - 1) * c_size + c] = a[static_cast<std::size_t>(rows - 1) * c_size + c];
        }
    }

    // 2. Compute interior in parallel
    #pragma omp parallel for schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      const std::size_t row_offset = static_cast<std::size_t>(r) * c_size;
      for (int c = 1; c < cols - 1; ++c) {
        const std::size_t idx = row_offset + c;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                  a[idx - c_size] + a[idx + c_size]) / 8u;
      }
    }
    
    a.swap(b);
  }

  return checksum_u32(a);
}