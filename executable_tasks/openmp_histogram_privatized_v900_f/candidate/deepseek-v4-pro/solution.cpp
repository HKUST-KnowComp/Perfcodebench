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
  int nt = num_threads();
  omp_set_num_threads(nt);
  std::size_t n = input.size();
  std::size_t nbins = static_cast<std::size_t>(bins);

  std::vector<uint32_t> counts(nbins, 0u);
  // Per-thread private histogram storage: nt * nbins elements
  std::vector<uint32_t> priv_counts(nt * nbins, 0u);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset private arrays
    std::fill(priv_counts.begin(), priv_counts.end(), 0u);

    const uint8_t* data = input.data();

    #pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint32_t* local = priv_counts.data() + static_cast<std::size_t>(tid) * nbins;

      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        local[data[i]]++;
      }
    }

    // Merge thread-local histograms into global counts for this iteration
    std::fill(counts.begin(), counts.end(), 0u);
    for (int t = 0; t < nt; ++t) {
      const uint32_t* src = priv_counts.data() + static_cast<std::size_t>(t) * nbins;
      for (std::size_t b = 0; b < nbins; ++b) {
        counts[b] += src[b];
      }
    }
  }

  return checksum_u32(counts);
}