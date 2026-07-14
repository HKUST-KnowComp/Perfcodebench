#include "interface.h"
#include <omp.h>
#include <vector>
#include <algorithm>

namespace {
  int num_threads() {
    return std::max(1, std::min(8, omp_get_max_threads()));
  }
}

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  const int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  const std::size_t bins_sz = static_cast<std::size_t>(bins);
  std::vector<uint32_t> private_hist(nthreads * bins_sz, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(private_hist.begin(), private_hist.end(), 0u);

#pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
      const int tid = omp_get_thread_num();
      private_hist[tid * bins_sz + input[i]]++;
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t bin = 0; bin < bins_sz; ++bin) {
    uint32_t total = 0;
    for (int t = 0; t < nthreads; ++t) {
      total += private_hist[t * bins_sz + bin];
    }
    hash ^= static_cast<uint64_t>(total);
    hash *= 1099511628211ULL;
  }
  return hash;
}