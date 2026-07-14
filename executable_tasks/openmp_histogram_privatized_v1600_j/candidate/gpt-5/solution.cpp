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
  const int T = num_threads();
  omp_set_dynamic(0);
  omp_set_num_threads(T);

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);
  // Preallocate privatized histograms: one slice per thread
  std::vector<uint32_t> priv(static_cast<std::size_t>(T) * static_cast<std::size_t>(bins));

  const uint8_t* data = input.data();
  const std::size_t n = input.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset global counts for this iteration
    std::fill(counts.begin(), counts.end(), 0u);

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      // Use the actually running number of threads in this region
      const int nt = omp_get_num_threads();
      // Ensure we map threads into the preallocated private buffer
      const int priv_threads = (nt <= T) ? nt : T;
      const int use_tid = (tid < priv_threads) ? tid : (priv_threads - 1);

      uint32_t* local = priv.data() + static_cast<std::size_t>(use_tid) * static_cast<std::size_t>(bins);
      // Zero this thread's private histogram
      for (int b = 0; b < bins; ++b) local[b] = 0u;

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        ++local[static_cast<std::size_t>(data[i])];
      }

#pragma omp barrier
#pragma omp single
      {
        // Reduce per-thread histograms into global counts
        for (int t = 0; t < priv_threads; ++t) {
          const uint32_t* src = priv.data() + static_cast<std::size_t>(t) * static_cast<std::size_t>(bins);
          for (int b = 0; b < bins; ++b) {
            counts[static_cast<std::size_t>(b)] += src[static_cast<std::size_t>(b)];
          }
        }
      }
    }
  }

  return checksum_u32(counts);
}
