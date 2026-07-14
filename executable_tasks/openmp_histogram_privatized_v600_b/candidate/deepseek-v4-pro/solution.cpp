#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstdint>

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
  const int max_nt = num_threads();
  omp_set_num_threads(max_nt);

  // allocate per-thread private histograms (max possible threads)
  std::vector<std::vector<uint32_t>> thread_hists(
      static_cast<std::size_t>(max_nt),
      std::vector<uint32_t>(static_cast<std::size_t>(bins), 0));

  // global histogram
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);

  for (int iter = 0; iter < iters; ++iter) {
    // reset global counts (will be filled by reduction)
    std::fill(counts.begin(), counts.end(), 0u);

#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      int nt  = omp_get_num_threads();   // actual number of threads in this team
      auto& local = thread_hists[static_cast<std::size_t>(tid)];

      // zero this thread's private histogram
      std::fill(local.begin(), local.end(), 0);

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        local[static_cast<std::size_t>(input[i])]++;
      }

      // reduction: one thread merges all private histograms into counts
#pragma omp single
      {
        // loop over all active threads
        for (int t = 0; t < nt; ++t) {
          const auto& src = thread_hists[static_cast<std::size_t>(t)];
          for (std::size_t b = 0; b < static_cast<std::size_t>(bins); ++b) {
            counts[b] += src[b];
          }
        }
      }
    }
  }

  return checksum_u32(counts);
}
