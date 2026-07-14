#include "interface.h"

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  if (iters == 0) return 0;
  const size_t N = signal.size();
  const size_t M = taps.size();
  const int16_t* sig = signal.data();
  const int16_t* tap = taps.data();
  uint64_t total = 1469598103934665603ULL;
  for (size_t i = 0; i < N; ++i) {
    int64_t acc = 0;
    size_t max_j = i < M ? i + 1 : M;
    for (size_t j = 0; j < max_j; ++j) {
      acc += static_cast<int64_t>(sig[i - j]) * static_cast<int64_t>(tap[j]);
    }
    total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
    total *= 1099511628211ULL;
  }
  return total;
}