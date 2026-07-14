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
  if (iters <= 0) {
    return checksum_u32(std::vector<uint32_t>(static_cast<std::size_t>(std::max(0, bins)), 0u));
  }

  const std::size_t num_bins = static_cast<std::size_t>(bins);
  std::vector<uint32_t> counts(num_bins, 0u);
  const std::size_t n = input.size();

  int nthreads = num_threads();
  omp_set_num_threads(nthreads);  // ensure we have exactly that many threads
  std::vector<std::vector<uint32_t>> locals(static_cast<std::size_t>(nthreads),
                                             std::vector<uint32_t>(num_bins, 0u));

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::vector<uint32_t>& local = locals[static_cast<std::size_t>(tid)];
    #pragma omp for schedule(static) nowait
    for (std::size_t i = 0; i < n; ++i) {
      ++local[static_cast<std::size_t>(input[i])];
    }
  }

  for (const auto& local : locals) {
    for (std::size_t j = 0; j < num_bins; ++j) {
      counts[j] += local[j];
    }
  }

  return checksum_u32(counts);
}