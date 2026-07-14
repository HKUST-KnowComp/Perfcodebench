#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int /*iters*/) {
  uint64_t total = 1469598103934665603ULL;
  const std::size_t N = signal.size();
  const std::size_t M = taps.size();
  for (std::size_t i = 0; i < N; ++i) {
    int64_t acc = 0;
    std::size_t j_max = (i < M) ? i : M - 1;
    for (std::size_t j = 0; j <= j_max; ++j) {
      acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
    }
    total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
    total *= 1099511628211ULL;
  }
  return total;
}
