#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <cstddef>
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
  int n = num_threads();
  omp_set_num_threads(n);

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);

  if (iters > 0) {
    // Privatized thread-local histograms
    std::vector<std::vector<uint32_t>> local_counts(n, std::vector<uint32_t>(bins, 0));

    #pragma omp parallel num_threads(n)
    {
      int tid = omp_get_thread_num();
      #pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < input.size(); ++i) {
        local_counts[tid][static_cast<std::size_t>(input[i])]++;
      }
    }

    // Combine per-thread histograms
    for (int t = 0; t < n; ++t) {
      for (int b = 0; b < bins; ++b) {
        counts[b] += local_counts[t][b];
      }
    }
  }

  return checksum_u32(counts);
}
