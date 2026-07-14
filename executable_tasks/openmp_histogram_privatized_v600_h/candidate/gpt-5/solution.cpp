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

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);
  // Privatized histograms: nthreads stripes of length bins
  std::vector<uint32_t> priv(static_cast<std::size_t>(nthreads) * static_cast<std::size_t>(bins), 0u);

  for (int iter = 0; iter < iters; ++iter) {
    // Compute histogram using privatized bins and parallel reduction over bins
#pragma omp parallel num_threads(nthreads)
    {
      const int tid = omp_get_thread_num();
      uint32_t* local = priv.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);

      // Zero this thread's private bins
      for (int b = 0; b < bins; ++b) {
        local[b] = 0u;
      }

      // Build local histogram
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        ++local[static_cast<unsigned>(input[i])];
      }

      // Reduce privatized histograms into shared counts in parallel over bins
#pragma omp for schedule(static)
      for (int b = 0; b < bins; ++b) {
        uint32_t sum = 0u;
        for (int t = 0; t < nthreads; ++t) {
          sum += priv[static_cast<std::size_t>(t) * static_cast<std::size_t>(bins) + static_cast<std::size_t>(b)];
        }
        counts[static_cast<std::size_t>(b)] = sum;
      }
    }
  }

  return checksum_u32(counts);
}
