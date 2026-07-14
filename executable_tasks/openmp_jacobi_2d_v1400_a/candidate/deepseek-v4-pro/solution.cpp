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
  std::vector<uint32_t> b(a.size());

  for (int step = 0; step < steps; ++step) {
    const uint32_t* a_ptr = a.data();
    uint32_t* b_ptr = b.data();

    // copy all border elements (they never change)
    // top row
    for (int c = 0; c < cols; ++c) {
      b_ptr[c] = a_ptr[c];
    }
    // bottom row
    for (int c = 0; c < cols; ++c) {
      b_ptr[(rows - 1) * cols + c] = a_ptr[(rows - 1) * cols + c];
    }
    // left and right columns (excluding corners)
    for (int r = 1; r < rows - 1; ++r) {
      b_ptr[r * cols] = a_ptr[r * cols];
      b_ptr[r * cols + cols - 1] = a_ptr[r * cols + cols - 1];
    }

    // interior points – parallelize only when grid is large enough
#pragma omp parallel for collapse(2) if(rows * cols > 4096) schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      for (int c = 1; c < cols - 1; ++c) {
        std::size_t idx = static_cast<std::size_t>(r) * cols + static_cast<std::size_t>(c);
        b_ptr[idx] = (a_ptr[idx] * 4u + a_ptr[idx - 1] + a_ptr[idx + 1] +
                      a_ptr[idx - static_cast<std::size_t>(cols)] +
                      a_ptr[idx + static_cast<std::size_t>(cols)]) /
                     8u;
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}