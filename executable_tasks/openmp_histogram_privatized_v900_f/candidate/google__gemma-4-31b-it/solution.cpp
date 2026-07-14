#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

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

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  int max_threads = omp_get_max_threads();
  // Use a fixed number of threads to avoid overhead and ensure stability
  int threads = std::max(1, std::min(8, max_threads));
  omp_set_num_threads(threads);

  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);
  
  // Privatization buffer: threads x bins
  // We align the rows to avoid false sharing between threads
  // 64 bytes is standard cache line size. bins is 256, so 256 * 4 = 1024 bytes (already aligned).
  std::vector<uint32_t> priv_counts(static_cast<std::size_t>(threads) * bins, 0);

  const uint8_t* data_ptr = input.data();
  const size_t n = input.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset privatization buffers
    std::fill(priv_counts.begin(), priv_counts.end(), 0u);

    #pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* local_hist = &priv_counts[tid * bins];

      #pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        local_hist[data_ptr[i]]++;
      }
    }

    // Merge local histograms into global_counts
    std::fill(global_counts.begin(), global_counts.end(), 0u);
    for (int t = 0; t < threads; ++t) {
      for (int b = 0; b < bins; ++b) {
        global_counts[b] += priv_counts[t * bins + b];
      }
    }
  }

  return checksum_u32(global_counts);
}