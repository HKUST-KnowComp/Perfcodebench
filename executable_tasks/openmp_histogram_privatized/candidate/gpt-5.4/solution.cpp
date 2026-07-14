#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline int num_threads() {
  int mt = omp_get_max_threads();
  if (mt < 1) return 1;
  return mt > 8 ? 8 : mt;
}

static inline uint64_t checksum_u32_ptr(const uint32_t* data, int n) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  omp_set_num_threads(num_threads());

  if (bins <= 0) {
    return 1469598103934665603ULL;
  }

  const int threads = num_threads();
  const std::size_t n = input.size();

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);

  if (bins == 256) {
    constexpr int PAD = 64;  // avoid false sharing between thread-local histograms
    std::vector<uint32_t> local(static_cast<std::size_t>(threads) * PAD, 0u);

    for (int iter = 0; iter < iters; ++iter) {
      for (std::size_t i = 0; i < local.size(); ++i) local[i] = 0u;

#pragma omp parallel
      {
        const int tid = omp_get_thread_num();
        uint32_t* hist = local.data() + static_cast<std::size_t>(tid) * PAD;

#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++hist[input[i]];
        }
      }

      for (int b = 0; b < 256; ++b) {
        uint32_t sum = 0;
        for (int t = 0; t < threads; ++t) {
          sum += local[static_cast<std::size_t>(t) * PAD + b];
        }
        counts[static_cast<std::size_t>(b)] = sum;
      }
    }

    return checksum_u32_ptr(counts.data(), 256);
  }

  std::vector<uint32_t> local(static_cast<std::size_t>(threads) * static_cast<std::size_t>(bins), 0u);

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < local.size(); ++i) local[i] = 0u;

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* hist = local.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        const unsigned v = static_cast<unsigned>(input[i]);
        if (v < static_cast<unsigned>(bins)) {
          ++hist[v];
        }
      }
    }

    for (int b = 0; b < bins; ++b) {
      uint32_t sum = 0;
      for (int t = 0; t < threads; ++t) {
        sum += local[static_cast<std::size_t>(t) * static_cast<std::size_t>(bins) + static_cast<std::size_t>(b)];
      }
      counts[static_cast<std::size_t>(b)] = sum;
    }
  }

  return checksum_u32_ptr(counts.data(), bins);
}
