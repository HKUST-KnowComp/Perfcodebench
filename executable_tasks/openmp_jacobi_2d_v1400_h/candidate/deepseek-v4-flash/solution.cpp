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
    uint32_t* __restrict__ a_data = a.data();
    uint32_t* __restrict__ b_data = b.data();

#pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      int row_offset = row * cols;
      uint32_t* a_row = a_data + row_offset;
      uint32_t* b_row = b_data + row_offset;

      if (row == 0 || row == rows - 1) {
        // Entire boundary row: copy unchanged
        for (int col = 0; col < cols; ++col) {
          b_row[col] = a_row[col];
        }
      } else {
        // Interior row: handle boundaries and interior separately
        b_row[0] = a_row[0];  // left boundary
        b_row[cols - 1] = a_row[cols - 1];  // right boundary

        for (int col = 1; col < cols - 1; ++col) {
          b_row[col] = (a_row[col] * 4u
                        + a_row[col - 1]
                        + a_row[col + 1]
                        + a_row[col - cols]
                        + a_row[col + cols]) / 8u;
        }
      }
    }

    a.swap(b);
  }

  return checksum_u32(a);
}
