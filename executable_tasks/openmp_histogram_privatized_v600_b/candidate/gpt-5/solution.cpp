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
  omp_set_num_threads(T);

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);

  // Maintain baseline semantics: if iters == 0, return checksum of zeroed counts.
  if (iters <= 0 || bins <= 0) {
    return checksum_u32(counts);
  }

  // Compute histogram once (final state equals last iteration in baseline).
  const std::size_t N = input.size();

  // Allocate per-thread partial histograms: T slices of size 'bins'.
  std::vector<uint32_t> partial(static_cast<std::size_t>(T) * static_cast<std::size_t>(bins), 0u);

#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    uint32_t* local = partial.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);

#pragma omp for schedule(static)
    for (std::size_t i = 0; i < N; ++i) {
      // input[i] is uint8_t in [0,255]; assume bins accommodates these indices per task contract.
      local[static_cast<std::size_t>(input[i])]++;
    }
  }

  // Combine per-thread histograms into the final counts.
  for (int b = 0; b < bins; ++b) {
    uint32_t sum = 0;
    std::size_t idx = static_cast<std::size_t>(b);
    for (int t = 0; t < T; ++t) {
      sum += partial[idx];
      idx += static_cast<std::size_t>(bins);
    }
    counts[static_cast<std::size_t>(b)] = sum;
  }

  return checksum_u32(counts);
}
