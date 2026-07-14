#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const std::size_t N = signal.size();
  const std::size_t M = taps.size();
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;
    const std::size_t transient_end = (M < N) ? M : N;

    // Process initial transient (i < number of taps)
    for (std::size_t i = 0; i < transient_end; ++i) {
      int64_t acc = 0;
      for (std::size_t j = 0; j <= i; ++j) {
        acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
      }
      // Update checksum immediately to avoid storing output
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }

    // Process steady state where all taps are valid, no inner conditionals
    if (N > M) {
      for (std::size_t i = M; i < N; ++i) {
        int64_t acc = 0;
        for (std::size_t j = 0; j < M; ++j) {
          acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
        }
        total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
        total *= 1099511628211ULL;
      }
    }
  }
  return total;
}