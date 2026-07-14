#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const size_t total = static_cast<size_t>(rows) * static_cast<size_t>(cols);
  const size_t cols_sz = static_cast<size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);
  
  uint32_t* __restrict src = a.data();
  uint32_t* __restrict dst = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows (top and bottom)
    #pragma omp parallel for schedule(static)
    for (size_t col = 0; col < cols_sz; ++col) {
      dst[col] = src[col];
      dst[(rows - 1) * cols_sz + col] = src[(rows - 1) * cols_sz + col];
    }
    
    // Process interior rows with boundary columns
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_off = static_cast<size_t>(row) * cols_sz;
      const size_t row_above = row_off - cols_sz;
      const size_t row_below = row_off + cols_sz;
      
      // Left boundary
      dst[row_off] = src[row_off];
      // Right boundary
      dst[row_off + cols_sz - 1] = src[row_off + cols_sz - 1];
      
      // Interior columns - main computation
      for (size_t col = 1; col < cols_sz - 1; ++col) {
        const size_t idx = row_off + col;
        dst[idx] = (src[idx] * 4u + src[idx - 1] + src[idx + 1] +
                    src[row_above + col] + src[row_below + col]) / 8u;
      }
    }
    
    // Swap pointers
    uint32_t* tmp = src;
    src = dst;
    dst = tmp;
  }
  
  // Compute checksum - parallelized with reduction
  // Use thread-local hashing then combine
  const int max_threads = omp_get_max_threads();
  std::vector<uint64_t> partial_hashes(max_threads, 1469598103934665603ULL);
  
  #pragma omp parallel
  {
    const int tid = omp_get_thread_num();
    const int nthreads = omp_get_num_threads();
    
    const size_t chunk = (total + nthreads - 1) / nthreads;
    const size_t start = static_cast<size_t>(tid) * chunk;
    const size_t end = (start + chunk < total) ? start + chunk : total;
    
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = start; i < end; ++i) {
      hash ^= static_cast<uint64_t>(src[i]);
      hash *= 1099511628211ULL;
    }
    partial_hashes[tid] = hash;
  }
  
  // Sequential combination - must preserve order for correctness
  // Actually, we need to compute the hash in exact sequential order
  // So we compute sequentially but can still benefit from cache prefetch
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < total; ++i) {
    hash ^= static_cast<uint64_t>(src[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}