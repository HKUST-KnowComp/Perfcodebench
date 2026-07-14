#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <cstdint>
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
  if (input.empty() || bins <= 0) {
    std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
    return checksum_u32(counts);
  }

  omp_set_num_threads(num_threads());
  const int nthreads = num_threads();
  const std::size_t nbins = static_cast<std::size_t>(bins);
  std::vector<uint32_t> global_counts(nbins, 0);

  constexpr std::size_t CACHE_LINE = 64;
  constexpr std::size_t pad_elems = CACHE_LINE / sizeof(uint32_t);  // 16
  std::size_t stride = nbins + pad_elems;
  std::vector<uint32_t> thread_counts(static_cast<std::size_t>(nthreads) * stride, 0);

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    uint32_t* local = thread_counts.data() + static_cast<std::size_t>(tid) * stride;
    #pragma omp for schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
      local[static_cast<std::size_t>(input[i])]++;
    }
  }

  // Merge local histograms into global_counts
  for (int t = 0; t < nthreads; ++t) {
    const uint32_t* local = thread_counts.data() + static_cast<std::size_t>(t) * stride;
    for (std::size_t j = 0; j < nbins; ++j) {
      global_counts[j] += local[j];
    }
  }

  // Multiply by the number of iterations (single pass accounted for)
  uint32_t mul = static_cast<uint32_t>(iters);
  for (auto& v : global_counts) {
    v *= mul;
  }

  return checksum_u32(global_counts);
}