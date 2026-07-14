#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  const std::size_t total = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
  const std::size_t cols_sz = static_cast<std::size_t>(cols);
  
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(total);
  
  uint32_t* __restrict pa = a.data();
  uint32_t* __restrict pb = b.data();
  
  for (int step = 0; step < steps; ++step) {
    // Copy boundary rows (top and bottom)
    #pragma omp parallel for schedule(static)
    for (int col = 0; col < cols; ++col) {
      pb[col] = pa[col];
      pb[(rows - 1) * cols_sz + col] = pa[(rows - 1) * cols_sz + col];
    }
    
    // Process interior rows in parallel
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      const std::size_t row_above = row_offset - cols_sz;
      const std::size_t row_below = row_offset + cols_sz;
      
      // Copy left boundary
      pb[row_offset] = pa[row_offset];
      
      // Process interior columns with SIMD
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = row_offset + static_cast<std::size_t>(col);
        pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] +
                   pa[row_above + col] + pa[row_below + col]) / 8u;
      }
      
      // Copy right boundary
      pb[row_offset + cols_sz - 1] = pa[row_offset + cols_sz - 1];
    }
    
    // Swap pointers
    uint32_t* tmp = pa;
    pa = pb;
    pb = tmp;
  }
  
  // Compute checksum - use the correct final buffer
  const uint32_t* final_data = pa;
  
  // Parallel checksum computation
  // Split into chunks and compute partial hashes, then combine
  const int num_threads = omp_get_max_threads();
  std::vector<uint64_t> partial_hashes(num_threads, 1469598103934665603ULL);
  std::vector<std::size_t> chunk_starts(num_threads + 1);
  
  const std::size_t chunk_size = (total + num_threads - 1) / num_threads;
  for (int t = 0; t <= num_threads; ++t) {
    chunk_starts[t] = (t < num_threads) ? (static_cast<std::size_t>(t) * chunk_size) : total;
    if (chunk_starts[t] > total) chunk_starts[t] = total;
  }
  
  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::size_t start = chunk_starts[tid];
    std::size_t end = chunk_starts[tid + 1];
    uint64_t hash = 1469598103934665603ULL;
    
    for (std::size_t i = start; i < end; ++i) {
      hash ^= static_cast<uint64_t>(final_data[i]);
      hash *= 1099511628211ULL;
    }
    partial_hashes[tid] = hash;
  }
  
  // Sequential combination - must process in order for correct FNV-1a result
  // Actually, FNV hash is order-dependent, so we need sequential processing
  // But we can still benefit from cache prefetching with parallel work
  
  // For correctness, compute hash sequentially but with optimizations
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t unroll = 8;
  std::size_t i = 0;
  
  for (; i + unroll <= total; i += unroll) {
    hash ^= static_cast<uint64_t>(final_data[i]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(final_data[i + 1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(final_data[i + 2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(final_data[i + 3]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(final_data[i + 4]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(final_data[i + 5]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(final_data[i + 6]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(final_data[i + 7]);
    hash *= 1099511628211ULL;
  }
  
  for (; i < total; ++i) {
    hash ^= static_cast<uint64_t>(final_data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}