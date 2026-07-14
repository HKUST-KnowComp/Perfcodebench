#include "interface.h"

#include <algorithm>
#include <cstdint>
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
  int nth = num_threads();
  omp_set_num_threads(nth);

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);

  // Pre‑allocate per‑thread private histograms
  std::vector<std::vector<uint32_t>> priv(
      static_cast<std::size_t>(nth),
      std::vector<uint32_t>(static_cast<std::size_t>(bins), 0));

  for (int iter = 0; iter < iters; ++iter) {
    // Reset private histograms
    for (int t = 0; t < nth; ++t) {
      std::fill(priv[static_cast<std::size_t>(t)].begin(),
                priv[static_cast<std::size_t>(t)].end(), 0u);
    }

    // Parallel histogram computation
    const std::size_t n = input.size();
#pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      int tid = omp_get_thread_num();
      uint32_t* local = priv[static_cast<std::size_t>(tid)].data();
      ++local[input[i]];
    }

    // Combine private histograms into global counts
    std::fill(counts.begin(), counts.end(), 0u);
    for (int t = 0; t < nth; ++t) {
      const auto& hist = priv[static_cast<std::size_t>(t)];
      for (int b = 0; b < bins; ++b) {
        counts[static_cast<std::size_t>(b)] += hist[static_cast<std::size_t>(b)];
      }
    }
  }

  return checksum_u32(counts);
}
