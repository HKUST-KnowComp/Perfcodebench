#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  if (iters <= 0) return 0;

  const int16_t* sig = signal.data();
  const int16_t* tap = taps.data();
  std::size_t N = signal.size();
  std::size_t M = taps.size();

  uint64_t total = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  std::size_t boundary = (N < M) ? N : M;

  // Part 1: Ramp-up phase where the filter window extends before the signal start
  for (std::size_t i = 0; i < boundary; ++i) {
    int64_t acc = 0;
    for (std::size_t j = 0; j <= i; ++j) {
      acc += static_cast<int64_t>(sig[i - j]) * static_cast<int64_t>(tap[j]);
    }
    total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
    total *= prime;
  }

  // Part 2: Steady-state phase with full filter window
  for (std::size_t i = boundary; i < N; ++i) {
    int64_t acc = 0;
    for (std::size_t j = 0; j < M; ++j) {
      acc += static_cast<int64_t>(sig[i - j]) * static_cast<int64_t>(tap[j]);
    }
    total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
    total *= prime;
  }

  return total;
}