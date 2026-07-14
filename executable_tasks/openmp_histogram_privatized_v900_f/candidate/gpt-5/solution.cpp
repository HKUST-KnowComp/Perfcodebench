#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstddef>

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
  omp_set_num_threads(num_threads());

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);

  // Allocate space for per-thread histograms (flattened)
  const int max_threads = num_threads();
  const std::size_t B = static_cast<std::size_t>(bins);
  std::vector<uint32_t> locals(static_cast<std::size_t>(max_threads) * B, 0u);

  for (int iter = 0; iter < iters; ++iter) {
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      const int T = omp_get_num_threads();
      uint32_t* local = locals.data() + static_cast<std::size_t>(tid) * B;

      // Zero this thread's local histogram for this iteration
      for (std::size_t b = 0; b < B; ++b) {
        local[b] = 0u;
      }

      // Build local histogram
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        const unsigned int v = input[i];
        // Assumes bins >= 256 as per task; mirrors baseline behavior
        local[v] += 1u;
      }

      // Reduce per-thread histograms into global counts
#pragma omp for schedule(static)
      for (int b = 0; b < bins; ++b) {
        uint32_t sum = 0u;
        for (int t = 0; t < T; ++t) {
          sum += locals[static_cast<std::size_t>(t) * B + static_cast<std::size_t>(b)];
        }
        counts[static_cast<std::size_t>(b)] = sum;
      }
    }
  }

  return checksum_u32(counts);
}
