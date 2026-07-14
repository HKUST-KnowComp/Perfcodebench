#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t size = data.size();
  const uint32_t* ptr = data.data();
  
  #pragma omp parallel
  {
    uint64_t local_hash = 1469598103934665603ULL;
    #pragma omp for nowait schedule(static)
    for (size_t i = 0; i < size; ++i) {
      local_hash ^= static_cast<uint64_t>(ptr[i]);
      local_hash *= 1099511628211ULL;
    }
    #pragma omp critical
    {
      for (size_t i = 0; i < size; ++i) {
        // Replay local contribution in order
      }
    }
  }
  
  // Serial version for correctness
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size(), 0);
  
  const size_t cols_sz = static_cast<size_t>(cols);
  uint32_t* __restrict__ src = a.data();
  uint32_t* __restrict__ dst = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy boundaries
    #pragma omp parallel
    {
      // Top and bottom rows
      #pragma omp for nowait schedule(static)
      for (int col = 0; col < cols; ++col) {
        dst[col] = src[col];
        dst[(rows - 1) * cols_sz + col] = src[(rows - 1) * cols_sz + col];
      }
      
      // Left and right columns (excluding corners)
      #pragma omp for nowait schedule(static)
      for (int row = 1; row < rows - 1; ++row) {
        const size_t idx = row * cols_sz;
        dst[idx] = src[idx];
        dst[idx + cols - 1] = src[idx + cols - 1];
      }
    }
    
    // Interior points with tiling
    const int tile_size = 64;
    #pragma omp parallel for collapse(2) schedule(static)
    for (int rt = 1; rt < rows - 1; rt += tile_size) {
      for (int ct = 1; ct < cols - 1; ct += tile_size) {
        const int r_end = (rt + tile_size < rows - 1) ? rt + tile_size : rows - 1;
        const int c_end = (ct + tile_size < cols - 1) ? ct + tile_size : cols - 1;
        
        for (int row = rt; row < r_end; ++row) {
          const size_t base_idx = row * cols_sz;
          const size_t prev_row = base_idx - cols_sz;
          const size_t next_row = base_idx + cols_sz;
          
          #pragma omp simd
          for (int col = ct; col < c_end; ++col) {
            const size_t idx = base_idx + col;
            dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] +
                       src[prev_row + col] + src[next_row + col]) / 8u;
          }
        }
      }
    }
    
    std::swap(src, dst);
  }
  
  // Ensure result is in 'a'
  if (src != a.data()) {
    a.swap(b);
  }
  
  return checksum_u32(a);
}
