#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  if (iters <= 0) return 0;

  size_t N = signal.size();
  size_t M = taps.size();
  const int16_t* sig = signal.data();
  const int16_t* tp = taps.data();

  uint64_t total = 1469598103934665603ULL;
  size_t boundary = (N < M) ? N : M;

  // Phase 1: Partial convolution where tap window exceeds available signal history
  for (size_t i = 0; i < boundary; ++i) {
    int64_t acc = 0;
    for (size_t j = 0; j <= i; ++j) {
      acc += static_cast<int64_t>(sig[i - j]) * static_cast<int64_t>(tp[j]);
    }
    uint64_t u = static_cast<uint64_t>(acc);
    total ^= (u * u + 3 * u);
    total *= 1099511628211ULL;
  }

  // Phase 2: Full convolution with constant tap count (enables auto-vectorization)
  for (size_t i = boundary; i < N; ++i) {
    int64_t acc = 0;
    for (size_t j = 0; j < M; ++j) {
      acc += static_cast<int64_t>(sig[i - j]) * static_cast<int64_t>(tp[j]);
    }
    uint64_t u = static_cast<uint64_t>(acc);
    total ^= (u * u + 3 * u);
    total *= 1099511628211ULL;
  }

  return total;
}
