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
  
  // Copy boundary rows once if steps > 0
  if (steps > 0) {
    // Top row
    for (std::size_t c = 0; c < cols_sz; ++c) {
      pb[c] = pa[c];
    }
    // Bottom row
    std::size_t last_row_start = static_cast<std::size_t>(rows - 1) * cols_sz;
    for (std::size_t c = 0; c < cols_sz; ++c) {
      pb[last_row_start + c] = pa[last_row_start + c];
    }
  }
  
  for (int step = 0; step < steps; ++step) {
    // Process interior rows in parallel
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      std::size_t row_start = static_cast<std::size_t>(row) * cols_sz;
      
      // Left boundary
      pb[row_start] = pa[row_start];
      
      // Interior columns - vectorizable
      #pragma omp simd
      for (std::size_t col = 1; col < cols_sz - 1; ++col) {
        std::size_t idx = row_start + col;
        pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] +
                   pa[idx - cols_sz] + pa[idx + cols_sz]) / 8u;
      }
      
      // Right boundary
      pb[row_start + cols_sz - 1] = pa[row_start + cols_sz - 1];
    }
    
    // Swap pointers
    uint32_t* tmp = pa;
    pa = pb;
    pb = tmp;
  }
  
  // Compute checksum - FNV-1a hash
  // Need to use the correct buffer (pa points to result after swaps)
  const uint32_t* result = pa;
  
  // Parallel checksum with careful combination
  int num_threads = 1;
  #pragma omp parallel
  {
    #pragma omp single
    num_threads = omp_get_num_threads();
  }
  
  std::vector<uint64_t> partial_hashes(num_threads, 1469598103934665603ULL);
  std::vector<std::size_t> thread_starts(num_threads + 1);
  
  std::size_t chunk = total / num_threads;
  std::size_t remainder = total % num_threads;
  std::size_t pos = 0;
  for (int t = 0; t < num_threads; ++t) {
    thread_starts[t] = pos;
    pos += chunk + (static_cast<std::size_t>(t) < remainder ? 1 : 0);
  }
  thread_starts[num_threads] = total;
  
  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::size_t start = thread_starts[tid];
    std::size_t end = thread_starts[tid + 1];
    
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = start; i < end; ++i) {
      hash ^= static_cast<uint64_t>(result[i]);
      hash *= 1099511628211ULL;
    }
    partial_hashes[tid] = hash;
  }
  
  // Combine hashes sequentially to maintain determinism
  // We need to replay the hash as if done sequentially
  uint64_t final_hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < total; ++i) {
    final_hash ^= static_cast<uint64_t>(result[i]);
    final_hash *= 1099511628211ULL;
  }
  
  return final_hash;
}