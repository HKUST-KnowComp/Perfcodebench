#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const size_t N = signal.size();
  const size_t M = taps.size();
  if (N == 0 || M == 0 || iters <= 0) return 0;

  std::vector<int64_t> output(N);
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // FIR convolution
    if (M == 1) {                       // fast path for single-tap
      int64_t k = taps[0];
      for (size_t i = 0; i < N; ++i)
        output[i] = k * static_cast<int64_t>(signal[i]);
    } else {
      // Initialize first M outputs naively
      for (size_t i = 0; i < M && i < N; ++i) {
        int64_t acc = 0;
        for (size_t j = 0; j <= i; ++j)
          acc += static_cast<int64_t>(signal[i - j]) * taps[j];
        output[i] = acc;
      }

      // Sliding-window update for the rest
      for (size_t i = M; i < N; ++i) {
        int64_t acc = output[i - 1];
        acc += static_cast<int64_t>(signal[i]) * taps[0];
        for (size_t j = 1; j < M; ++j)
          acc -= static_cast<int64_t>(signal[i - j]) * taps[j];
        output[i] = acc;
      }
    }

    // Checksum
    total = 1469598103934665603ULL;
    const int64_t* __restrict p = output.data();
    const size_t N4 = N & ~size_t(3);
    for (size_t i = 0; i < N4; i += 4) {
      uint64_t v0 = static_cast<uint64_t>(p[i] * p[i] + 3 * p[i]);
      uint64_t v1 = static_cast<uint64_t>(p[i + 1] * p[i + 1] + 3 * p[i + 1]);
      uint64_t v2 = static_cast<uint64_t>(p[i + 2] * p[i + 2] + 3 * p[i + 2]);
      uint64_t v3 = static_cast<uint64_t>(p[i + 3] * p[i + 3] + 3 * p[i + 3]);
      total ^= v0; total *= 1099511628211ULL;
      total ^= v1; total *= 1099511628211ULL;
      total ^= v2; total *= 1099511628211ULL;
      total ^= v3; total *= 1099511628211ULL;
    }
    for (size_t i = N4; i < N; ++i) {
      uint64_t v = static_cast<uint64_t>(p[i] * p[i] + 3 * p[i]);
      total ^= v;
      total *= 1099511628211ULL;
    }
  }
  return total;
}