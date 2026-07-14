#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

// Helper to calculate the FNV-1a-like checksum provided in the baseline
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
  // Use a reasonable number of threads, but respect the environment's max.
  const int max_threads = omp_get_max_threads();
  const std::size_t input_size = input.size();
  const uint8_t* data_ptr = input.data();
  
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);

  // Move the parallel region outside the iters loop if possible to reduce fork/join overhead,
  // but since the baseline resets 'counts' every iteration, we must ensure correctness.
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);

    #pragma omp parallel num_threads(max_threads)
    {
      // Privatize the histogram: each thread gets its own local bin array.
      // 256 bins * 4 bytes = 1KB, which fits easily in L1 cache.
      // We use a fixed size for the common case (256) to allow compiler optimizations.
      uint32_t local_bins[256];
      std::memset(local_bins, 0, sizeof(local_bins));

      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input_size; ++i) {
        local_bins[data_ptr[i]]++;
      }

      // Merge local histograms into the global count.
      // Since bins is small (256), this critical section is very fast.
      #pragma omp critical
      {
        for (int b = 0; b < bins; ++b) {
          counts[b] += local_bins[b];
        }
      }
    }
  }

  return checksum_u32(counts);
}
