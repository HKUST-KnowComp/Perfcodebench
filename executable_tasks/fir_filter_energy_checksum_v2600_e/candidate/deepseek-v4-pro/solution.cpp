#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  if (iters <= 0) {
    return 0;
  }
  const size_t N = signal.size();
  const size_t M = taps.size();
  std::vector<int64_t> output(N, 0);
  const int16_t* sig = signal.data();
  const int16_t* t = taps.data();
  int64_t* out = output.data();
  for (size_t j = 0; j < M; ++j) {
    int64_t tap = static_cast<int64_t>(t[j]);
    for (size_t i = j; i < N; ++i) {
      out[i] += tap * static_cast<int64_t>(sig[i - j]);
    }
  }
  uint64_t total = 1469598103934665603ULL;
  for (size_t i = 0; i < N; ++i) {
    int64_t acc = out[i];
    uint64_t val = static_cast<uint64_t>(acc * acc + 3 * acc);
    total ^= val;
    total *= 1099511628211ULL;
  }
  return total;
}