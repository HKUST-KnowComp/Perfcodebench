#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

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
  const int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  // Preallocate private thread histograms once to avoid repeated allocation overhead
  std::vector<std::vector<uint32_t>> thread_local_counts(nthreads, std::vector<uint32_t>(bins, 0));

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);

    #pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      auto& local_counts = thread_local_counts[tid];
      std::fill(local_counts.begin(), local_counts.end(), 0u);

      // Process chunk of input with no synchronization needed for local increments
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        local_counts[static_cast<std::size_t>(input[i])]++;
      }

      // Merge local results into global counts once per thread, minimal synchronization
      #pragma omp critical
      {
        for (int b = 0; b < bins; ++b) {
          counts[b] += local_counts[b];
        }
      }
    }
  }
  return checksum_u32(counts);
}