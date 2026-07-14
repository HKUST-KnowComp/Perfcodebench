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
  int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  const std::size_t bin_count = static_cast<std::size_t>(bins);
  std::vector<uint32_t> global_counts(bin_count, 0);
  // Pre-allocate enough space for all threads' private histograms.
  std::vector<uint32_t> local_all(static_cast<std::size_t>(nthreads) * bin_count, 0);
  for (int iter = 0; iter < iters; ++iter) {
    // Zero the whole local buffer
    std::fill(local_all.begin(), local_all.end(), 0u);
    #pragma omp parallel num_threads(nthreads)
    {
      int tid = omp_get_thread_num();
      uint32_t* my_bins = local_all.data() + static_cast<std::size_t>(tid) * bin_count;
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        ++my_bins[static_cast<std::size_t>(input[i])];
      }
    }
    // Reduce all thread histograms into global_counts
    std::fill(global_counts.begin(), global_counts.end(), 0u);
    for (int t = 0; t < nthreads; ++t) {
      const uint32_t* src = local_all.data() + static_cast<std::size_t>(t) * bin_count;
      for (std::size_t b = 0; b < bin_count; ++b) {
        global_counts[b] += src[b];
      }
    }
  }
  return checksum_u32(global_counts);
}