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

  std::size_t nbins = static_cast<std::size_t>(bins);
  std::vector<uint32_t> counts(nbins, 0);

  // Preallocate per-thread private histograms in a single contiguous buffer
  std::vector<uint32_t> priv_counts(static_cast<std::size_t>(nthreads) * nbins, 0);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset global counts
    std::fill(counts.begin(), counts.end(), 0u);
    // Reset private histograms
    std::fill(priv_counts.begin(), priv_counts.end(), 0u);

    // Parallel histogram with privatized arrays – no atomic inside the loop
    #pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
      int tid = omp_get_thread_num();
      priv_counts[static_cast<std::size_t>(tid) * nbins + static_cast<std::size_t>(input[i])]++;
    }

    // Merge private histograms into global counts (no locks needed, exclusive sections)
    for (int t = 0; t < nthreads; ++t) {
      std::size_t base = static_cast<std::size_t>(t) * nbins;
      for (std::size_t b = 0; b < nbins; ++b) {
        counts[b] += priv_counts[base + b];
      }
    }
  }

  return checksum_u32(counts);
}
