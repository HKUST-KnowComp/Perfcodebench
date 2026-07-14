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
  
  // Create thread-private histogram arrays to eliminate atomic contention
  std::vector<std::vector<uint32_t>> private_counts(nthreads, std::vector<uint32_t>(static_cast<std::size_t>(bins), 0));

  for (int iter = 0; iter < iters; ++iter) {
    // Reset all private counts for new iteration
    for (auto& pc : private_counts) {
      std::fill(pc.begin(), pc.end(), 0u);
    }
    std::fill(counts.begin(), counts.end(), 0u);

    // Parallel processing with thread-local counts, no atomics needed
    #pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        private_counts[tid][static_cast<std::size_t>(input[i])]++;
      }
    }

    // Merge per-thread results into global counts
    for (int t = 0; t < nthreads; ++t) {
      for (std::size_t b = 0; b < static_cast<std::size_t>(bins); ++b) {
        counts[b] += private_counts[t][b];
      }
    }
  }

  return checksum_u32(counts);
}