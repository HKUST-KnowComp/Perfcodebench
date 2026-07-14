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
  std::vector<uint32_t> b(a.size(), 0);

  const std::size_t row_size = static_cast<std::size_t>(cols);
  const std::size_t total_size = static_cast<std::size_t>(rows) * row_size;

  // Single parallel region to amortize thread creation over all steps
#pragma omp parallel
  {
    for (int step = 0; step < steps; ++step) {
#pragma omp for schedule(static)
      for (int row = 0; row < rows; ++row) {
        const std::size_t base = static_cast<std::size_t>(row) * row_size;

        if (row == 0 || row == rows - 1) {
          // Boundary rows: copy entire row unchanged
          for (std::size_t col = 0; col < row_size; ++col) {
            b[base + col] = a[base + col];
          }
        } else {
          // Interior rows: handle corners explicitly, stencil for interior columns
          const std::size_t left = base;
          const std::size_t right = base + row_size - 1;
          b[left] = a[left];
          b[right] = a[right];

          for (std::size_t col = 1; col < row_size - 1; ++col) {
            const std::size_t idx = base + col;
            b[idx] = (a[idx] * 4u +
                      a[idx - 1] +
                      a[idx + 1] +
                      a[idx - row_size] +
                      a[idx + row_size]) / 8u;
          }
        }
      } // implicit barrier here

      // Only one thread swaps buffers after all work is done
#pragma omp single
      { a.swap(b); }
    } // end for step
  } // end parallel region

  return checksum_u32(a);
}
