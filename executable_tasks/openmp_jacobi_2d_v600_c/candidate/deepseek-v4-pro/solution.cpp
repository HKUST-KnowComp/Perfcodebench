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
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows and columns to the output buffer
    #pragma omp parallel for schedule(static)
    for (int r = 0; r < rows; ++r) {
      if (r == 0 || r == rows - 1) {
        // full rows
        uint32_t* a_row = a.data() + static_cast<std::size_t>(r) * cols;
        uint32_t* b_row = b.data() + static_cast<std::size_t>(r) * cols;
        for (int c = 0; c < cols; ++c) {
          b_row[c] = a_row[c];
        }
      } else {
        // first and last column of interior rows
        std::size_t base = static_cast<std::size_t>(r) * cols;
        b[base] = a[base];
        b[base + cols - 1] = a[base + cols - 1];
      }
    }

    // Update interior points using the Jacobi stencil
    #pragma omp parallel for collapse(2) schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      for (int c = 1; c < cols - 1; ++c) {
        const std::size_t idx = static_cast<std::size_t>(r) * cols + c;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - cols] + a[idx + cols]) / 8u;
      }
    }
    a.swap(b);
  }
  return checksum_u32(a);
}