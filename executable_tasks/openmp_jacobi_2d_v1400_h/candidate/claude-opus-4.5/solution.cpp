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
      const std::size_t row_offset = static_cast<std::size_t>(row) * cols_sz;
      const std::size_t row_above = row_offset - cols_sz;
      const std::size_t row_below = row_offset + cols_sz;
      
      // Left boundary
      pb[row_offset] = pa[row_offset];
      
      // Interior columns - vectorizable loop
      #pragma omp simd
      for (int col = 1; col < cols - 1; ++col) {
        const std::size_t idx = row_offset + static_cast<std::size_t>(col);
        pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] +
                   pa[row_above + static_cast<std::size_t>(col)] +
                   pa[row_below + static_cast<std::size_t>(col)]) / 8u;
      }
      
      // Right boundary
      pb[row_offset + cols_sz - 1] = pa[row_offset + cols_sz - 1];
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
  
  std::vector<uint64_t> partial_hashes(static_cast<std::size_t>(num_threads));
  std::vector<std::size_t> thread_starts(static_cast<std::size_t>(num_threads));
  std::vector<std::size_t> thread_ends(static_cast<std::size_t>(num_threads));
  
  // Precompute ranges
  std::size_t chunk = total / static_cast<std::size_t>(num_threads);
  for (int t = 0; t < num_threads; ++t) {
    thread_starts[static_cast<std::size_t>(t)] = static_cast<std::size_t>(t) * chunk;
    thread_ends[static_cast<std::size_t>(t)] = (t == num_threads - 1) ? total : (static_cast<std::size_t>(t) + 1) * chunk;
  }
  
  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::size_t start = thread_starts[static_cast<std::size_t>(tid)];
    std::size_t end = thread_ends[static_cast<std::size_t>(tid)];
    
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = start; i < end; ++i) {
      hash ^= static_cast<uint64_t>(result[i]);
      hash *= 1099511628211ULL;
    }
    partial_hashes[static_cast<std::size_t>(tid)] = hash;
  }
  
  // Sequential combination to preserve order
  // We need to properly chain the hashes
  // Actually, the original hash is sequential, so we need to compute it sequentially
  // or use a different approach. Let's just do it sequentially for correctness.
  
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < total; ++i) {
    hash ^= static_cast<uint64_t>(result[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}