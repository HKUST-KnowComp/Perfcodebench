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
  const int nt = num_threads();
  omp_set_num_threads(nt);

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  // Per-thread local histograms.  Each row is bins elements,
  // which is exactly 256 * 4 = 1024 bytes -> 16 cache lines, so
  // rows are naturally cache-line aligned and false sharing is avoided.
  std::vector<uint32_t> local_counts(static_cast<std::size_t>(nt) * bins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset all local bins before the parallel loop.
    std::fill(local_counts.begin(), local_counts.end(), 0u);

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* my_counts = &local_counts[static_cast<std::size_t>(tid) * bins];

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        // No atomics needed; each thread writes exclusively to its own buffer.
        my_counts[static_cast<std::size_t>(input[i])]++;
      }
    } // Implicit barrier, then serial region

    // Reduce per-thread histograms into the global counts.
    // Counts is overwritten each iteration, so we fill it with zero first.
    std::fill(counts.begin(), counts.end(), 0u);
    for (int t = 0; t < nt; ++t) {
      const uint32_t* src = &local_counts[static_cast<std::size_t>(t) * bins];
      for (int b = 0; b < bins; ++b) {
        counts[static_cast<std::size_t>(b)] += src[b];
      }
    }
  }

  return checksum_u32(counts);
}
