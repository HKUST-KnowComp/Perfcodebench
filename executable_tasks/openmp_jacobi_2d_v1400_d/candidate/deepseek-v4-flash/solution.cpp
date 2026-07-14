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

  const auto* a_ptr = a.data();
  auto* b_ptr = b.data();
  const size_t rows_u = static_cast<size_t>(rows);
  const size_t cols_u = static_cast<size_t>(cols);
  const size_t stride = cols_u;

  #pragma omp parallel
  for (int step = 0; step < steps; ++step) {
    #pragma omp for schedule(static)
    for (int row = 0; row < rows; ++row) {
      const size_t row_offset = static_cast<size_t>(row) * stride;

      // Copy entire first and last row unchanged
      if (row == 0 || row == rows_u - 1) {
        for (size_t col = 0; col < cols_u; ++col) {
          b_ptr[row_offset + col] = a_ptr[row_offset + col];
        }
        continue;
      }

      // Left and right boundaries remain unchanged
      b_ptr[row_offset] = a_ptr[row_offset];
      b_ptr[row_offset + cols_u - 1] = a_ptr[row_offset + cols_u - 1];

      // Interior columns
      for (int col = 1; col < cols_u - 1; ++col) {
        const size_t idx = row_offset + col;
        b_ptr[idx] = ( (a_ptr[idx] << 2) +
                        a_ptr[idx - 1] +
                        a_ptr[idx + 1] +
                        a_ptr[idx - stride] +
                        a_ptr[idx + stride] ) >> 3;
      }
    }

    #pragma omp master
    {
      a.swap(b);
    }
    #pragma omp barrier

    // Update pointers after swap (necessary because swap invalidates pointers)
    a_ptr = a.data();
    b_ptr = b.data();
  }

  return checksum_u32(a);
}
