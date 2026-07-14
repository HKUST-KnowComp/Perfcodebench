#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);
  
  uint32_t* src = a.data();
  uint32_t* dst = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy top boundary row
    #pragma omp parallel for simd schedule(static)
    for (std::size_t col = 0; col < cols_sz; ++col) {
      dst[col] = src[col];
    }
    
    // Copy bottom boundary row
    const std::size_t last_row_offset = (static_cast<std::size_t>(rows) - 1) * cols_sz;
    #pragma omp parallel for simd schedule(static)
    for (std::size_t col = 0; col < cols_sz; ++col) {
      dst[last_row_offset + col] = src[last_row_offset + col];
    }
    
    // Process interior rows in parallel
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      
      // Left boundary
      dst[row_offset] = src[row_offset];
      
      // Interior columns - vectorizable
      #pragma omp simd
      for (std::size_t col = 1; col < cols_sz - 1; ++col) {
        const std::size_t idx = row_offset + col;
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] +
                    src[idx - cols_sz] + src[idx + cols_sz]) / 8u;
      }
      
      // Right boundary
      dst[row_offset + cols_sz - 1] = src[row_offset + cols_sz - 1];
    }
    
    // Swap pointers
    uint32_t* tmp = src;
    src = dst;
    dst = tmp;
  }
  
  // Parallel checksum computation
  const int num_threads = omp_get_max_threads();
  std::vector<uint64_t> partial_hashes(num_threads, 1469598103934665603ULL);
  
  #pragma omp parallel
  {
    const int tid = omp_get_thread_num();
    const int nthreads = omp_get_num_threads();
    
    const std::size_t chunk = (total + nthreads - 1) / nthreads;
    const std::size_t start = static_cast<std::size_t>(tid) * chunk;
    const std::size_t end = (start + chunk < total) ? start + chunk : total;
    
    uint64_t local_hash = 1469598103934665603ULL;
    for (std::size_t i = start; i < end; ++i) {
      local_hash ^= static_cast<uint64_t>(src[i]);
      local_hash *= 1099511628211ULL;
    }
    partial_hashes[tid] = local_hash;
  }
  
  // Combine partial hashes sequentially (order matters for FNV)
  uint64_t final_hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < total; ++i) {
    final_hash ^= static_cast<uint64_t>(src[i]);
    final_hash *= 1099511628211ULL;
  }
  
  return final_hash;
}