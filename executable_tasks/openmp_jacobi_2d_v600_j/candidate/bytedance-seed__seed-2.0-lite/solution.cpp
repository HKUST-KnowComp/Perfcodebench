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

  if (rows <= 1 || cols <= 1 || steps == 0) {
    return checksum_u32(a);
  }

  const size_t cols_s = static_cast<size_t>(cols);
  const size_t rows_s = static_cast<size_t>(rows);

  // Initialize boundaries once, they never change
  // First row (row 0)
  for (int col = 0; col < cols; ++col) {
    b[col] = a[col];
  }
  // Last row
  const size_t last_row_base = (rows_s - 1) * cols_s;
  for (int col = 0; col < cols; ++col) {
    b[last_row_base + col] = a[last_row_base + col];
  }
  // First and last column of middle rows
  for (int row = 1; row < rows - 1; ++row) {
    const size_t row_base = static_cast<size_t>(row) * cols_s;
    b[row_base] = a[row_base];
    b[row_base + cols_s - 1] = a[row_base + cols_s - 1];
  }

  // Run Jacobi steps
  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_base = static_cast<size_t>(row) * cols_s;
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = row_base + col;
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] + a[idx - cols_s] + a[idx + cols_s]) / 8u;
      }
    }
    a.swap(b);
  }

  return checksum_u32(a);
}