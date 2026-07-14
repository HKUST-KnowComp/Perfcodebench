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
    #pragma omp parallel for collapse(2)
    for (int row = 1; row < rows - 1; ++row) {
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = static_cast<size_t>(row) * stride + col;
        p_b[idx] = (p_a[idx] * 4u + p_a[idx - 1] + p_a[idx + 1] + 
                    p_a[idx - stride] + p_a[idx + stride]) / 8u;
      }
    }
    
    // Boundary values remain constant in this specific Jacobi variant
    // Since b was initialized with input and a is swapped, 
    // the boundaries in b are already correct if we don't overwrite them.
    // However, to be strictly correct with the original logic where b[idx] = a[idx],
    // and since boundaries never change, we only need to ensure they are set once.
    
    std::swap(p_a, p_b);
  }

  // The final result is in p_a
  if (p_a != a.data()) {
    a = std::vector<uint32_t>(p_b, p_b + (rows * cols)); // This is slow, better to handle pointers
    // To avoid vector copy, we can just use the pointer for checksum if we change checksum_u32
  }
  
  // Re-calculating checksum using the pointer to avoid vector copies
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* final_data = p_a;
  size_t total_size = static_cast<size_t>(rows) * stride;
  for (size_t i = 0; i < total_size; ++i) {
    hash ^= static_cast<uint64_t>(final_data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}