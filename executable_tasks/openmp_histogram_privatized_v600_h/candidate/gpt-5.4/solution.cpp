#include "interface.h"

#include <omp.h>
#include <cstdint>
#include <vector>

namespace {

inline int tuned_num_threads() {
  int t = omp_get_max_threads();
  if (t < 1) return 1;
  if (t > 8) return 8;
  return t;
}

inline uint64_t checksum_u32_ptr(const uint32_t* data, int n) {
  uint64_t hash = 1469598103934665603ULL;
  for (int i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  omp_set_num_threads(tuned_num_threads());

  if (bins <= 0) {
    return 1469598103934665603ULL;
  }

  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0u);
  if (iters <= 0) {
    return checksum_u32_ptr(counts.data(), bins);
  }

  const uint8_t* in = input.empty() ? nullptr : input.data();
  const std::size_t n = input.size();

  // Fast path for the task's intended configuration: 256 bins over uint8 input.
  if (bins == 256) {
    const int max_threads = tuned_num_threads();
    std::vector<uint32_t> priv(static_cast<std::size_t>(max_threads) * 256u, 0u);

    for (int iter = 0; iter < iters; ++iter) {
      uint32_t* counts_ptr = counts.data();
      for (int i = 0; i < 256; ++i) counts_ptr[i] = 0u;

#pragma omp parallel
      {
        const int tid = omp_get_thread_num();
        uint32_t* local = priv.data() + static_cast<std::size_t>(tid) * 256u;

        for (int i = 0; i < 256; ++i) local[i] = 0u;

#pragma omp for schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
          ++local[in[i]];
        }

#pragma omp for schedule(static)
        for (int b = 0; b < 256; ++b) {
          uint32_t sum = 0u;
          for (int t = 0; t < max_threads; ++t) {
            sum += priv[static_cast<std::size_t>(t) * 256u + static_cast<std::size_t>(b)];
          }
          counts_ptr[b] = sum;
        }
      }
    }

    return checksum_u32_ptr(counts.data(), 256);
  }

  // Generic fallback for arbitrary bin counts while preserving correctness.
  const int max_threads = tuned_num_threads();
  std::vector<uint32_t> priv(static_cast<std::size_t>(max_threads) * static_cast<std::size_t>(bins), 0u);

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t* counts_ptr = counts.data();
    for (int b = 0; b < bins; ++b) counts_ptr[b] = 0u;

#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint32_t* local = priv.data() + static_cast<std::size_t>(tid) * static_cast<std::size_t>(bins);

      for (int b = 0; b < bins; ++b) local[b] = 0u;

#pragma omp for schedule(static)
      for (std::size_t i = 0; i < n; ++i) {
        const std::size_t idx = static_cast<std::size_t>(in[i]);
        if (idx < static_cast<std::size_t>(bins)) {
          ++local[idx];
        }
      }

#pragma omp for schedule(static)
      for (int b = 0; b < bins; ++b) {
        uint32_t sum = 0u;
        for (int t = 0; t < max_threads; ++t) {
          sum += priv[static_cast<std::size_t>(t) * static_cast<std::size_t>(bins) + static_cast<std::size_t>(b)];
        }
        counts_ptr[b] = sum;
      }
    }
  }

  return checksum_u32_ptr(counts.data(), bins);
}
