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
  // The problem specifies 256 bins. We use a fixed size for local buffers to avoid allocation.
  const int MAX_BINS = 256;
  std::vector<uint32_t> global_counts(static_cast<std::size_t>(bins), 0);
  const uint8_t* data_ptr = input.data();
  const std::size_t n = input.size();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0u);

    #pragma omp parallel
    {
      // Privatize histogram to avoid atomic contention. 
      // Aligned to cache line to prevent false sharing between threads.
      alignas(64) uint32_t local_counts[MAX_BINS] = {0};

      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        local_counts[data_ptr[i]]++;
      }

      #pragma omp critical
      {
        for (int b = 0; b < bins; ++b) {
          global_counts[b] += local_counts[b];
        }
      }
    }
  }

  return checksum_u32(global_counts);
}