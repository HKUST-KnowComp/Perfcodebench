#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>

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
    uint32_t* cur = a.data();
    uint32_t* nxt = b.data();

    // Copy top and bottom rows
    for (int c = 0; c < cols; ++c) {
      nxt[c] = cur[c];
      nxt[(rows - 1) * cols + c] = cur[(rows - 1) * cols + c];
    }
    // Copy left and right columns
    for (int r = 1; r < rows - 1; ++r) {
      nxt[r * cols] = cur[r * cols];
      nxt[r * cols + cols - 1] = cur[r * cols + cols - 1];
    }

    #pragma omp parallel for schedule(static)
    for (int r = 1; r < rows - 1; ++r) {
      const uint32_t* a_row = cur + r * cols;
      uint32_t* b_row = nxt + r * cols;
      const uint32_t* a_up = a_row - cols;
      const uint32_t* a_down = a_row + cols;
      for (int c = 1; c < cols - 1; ++c) {
        b_row[c] = (a_row[c] * 4u + a_row[c-1] + a_row[c+1] + a_up[c] + a_down[c]) >> 3;
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}
