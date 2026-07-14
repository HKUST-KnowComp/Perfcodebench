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
  
  const std::size_t c_size = static_cast<std::size_t>(cols);
  const std::size_t total_size = static_cast<std::size_t>(rows) * c_size;

  for (int step = 0; step < steps; ++step) {
    // Handle boundaries first (they remain constant in value from 'a' to 'b')
    // Top and Bottom rows
    #pragma omp parallel for
    for (std::size_t col = 0; col < c_size; ++col) {
      b[col] = a[col]; // Top row
      b[static_cast<std::size_t>(rows - 1) * c_size + col] = a[static_cast<std::size_t>(rows - 1) * c_size + col]; // Bottom row
    }

    // Left and Right columns (excluding corners already handled)
    #pragma omp parallel for
    for (int row = 1; row < rows - 1; ++row) {
      std::size_t r_idx = static_cast<std::size_t>(row) * c_size;
      b[r_idx] = a[r_idx]; // Left col
      b[r_idx + c_size - 1] = a[r_idx + c_size - 1]; // Right col
    }

    // Core computation: avoid branches inside the loop
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t r_idx = static_cast<std::size_t>(row) * c_size;
      for (std::size_t col = 1; col < c_size - 1; ++col) {
        const std::size_t idx = r_idx + col;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + 
                  a[idx - c_size] + a[idx + c_size]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}