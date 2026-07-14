#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  
  #pragma omp parallel
  {
    uint64_t local_hash = 1469598103934665603ULL;
    
    #pragma omp for nowait schedule(static)
    for (std::size_t i = 0; i < data.size(); ++i) {
      local_hash ^= static_cast<uint64_t>(data[i]);
      local_hash *= 1099511628211ULL;
    }
    
    #pragma omp critical
    {
      for (std::size_t i = 0; i < data.size(); ++i) {
        uint64_t v = data[i];
        #pragma omp atomic
        hash ^= v;
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
  
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel
    {
      // Top and bottom boundaries
      #pragma omp for schedule(static) nowait
      for (int col = 0; col < cols; ++col) {
        b[col] = a[col];
        b[(rows - 1) * cols_sz + col] = a[(rows - 1) * cols_sz + col];
      }
      
      // Interior rows with left/right boundaries
      #pragma omp for schedule(static)
      for (int row = 1; row < rows - 1; ++row) {
        const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
        
        // Left boundary
        b[row_offset] = a[row_offset];
        
        // Interior columns
        for (int col = 1; col < cols - 1; ++col) {
          const std::size_t idx = row_offset + col;
          b[idx] = (a[idx] * 4u + a[idx - 1] + a[idx + 1] +
                    a[idx - cols_sz] + a[idx + cols_sz]) / 8u;
        }
        
        // Right boundary
        b[row_offset + cols - 1] = a[row_offset + cols - 1];
      }
    }
    
    a.swap(b);
  }
  
  return checksum_u32(a);
}
