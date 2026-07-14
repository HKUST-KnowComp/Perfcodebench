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
  omp_set_num_threads(num_threads());
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  const int threads = num_threads();
  std::vector<uint32_t> locals(static_cast<std::size_t>(threads) * static_cast<std::size_t>(bins), 0u);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    std::fill(locals.begin(), locals.end(), 0u);
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* local = locals.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);
#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        local[static_cast<std::size_t>(input[i])]++;
      }
    }
    for (int tid = 0; tid < threads; ++tid) {
      const uint32_t* local = locals.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);
      for (int bin = 0; bin < bins; ++bin) {
        counts[static_cast<std::size_t>(bin)] += local[static_cast<std::size_t>(bin)];
      }
    }
  }
  return checksum_u32(counts);
}
