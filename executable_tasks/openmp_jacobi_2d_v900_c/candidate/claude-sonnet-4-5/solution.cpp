#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = data.size();
  
  #pragma omp parallel
  {
    uint64_t local_hash = 1469598103934665603ULL;
    
    #pragma omp for nowait schedule(static)
    for (size_t i = 0; i < n; ++i) {
      local_hash ^= static_cast<uint64_t>(data[i]);
      local_hash *= 1099511628211ULL;
    }
    
    #pragma omp critical
    {
      for (size_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
      }
    }
  }
  
  hash = 1469598103934665603ULL;
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
  
  const size_t cols_sz = static_cast<size_t>(cols);
  
  for (int step = 0; step < steps; ++step) {
    // Copy boundaries
    #pragma omp parallel
    {
      // Top and bottom rows
      #pragma omp for schedule(static) nowait
      for (int col = 0; col < cols; ++col) {
        b[col] = a[col];
        b[(rows - 1) * cols_sz + col] = a[(rows - 1) * cols_sz + col];
      }
      
      // Left and right columns (excluding corners)
      #pragma omp for schedule(static) nowait
      for (int row = 1; row < rows - 1; ++row) {
        const size_t idx = row * cols_sz;
        b[idx] = a[idx];
        b[idx + cols - 1] = a[idx + cols - 1];
      }
    }
    
    // Interior points
    #pragma omp parallel for schedule(static) collapse(2)
    for (int row = 1; row < rows - 1; ++row) {
      for (int col = 1; col < cols - 1; ++col) {
        const size_t idx = static_cast<size_t>(row) * cols_sz + static_cast<size_t>(col);
        b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                  a[idx - cols_sz] + a[idx + cols_sz]) / 8u;
      }
    }
    
    a.swap(b);
  }
  
  return checksum_u32(a);
}
