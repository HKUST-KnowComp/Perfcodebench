#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const size_t n = signal.size();
  const size_t m = taps.size();
  if (n == 0) return 0;

  std::vector<int64_t> output(n);
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // FIR Filter implementation
    for (size_t i = 0; i < n; ++i) {
      int64_t acc = 0;
      // Handle ramp-up phase where j > i is possible
      size_t limit = (i < m) ? (i + 1) : m;
      for (size_t j = 0; j < limit; ++j) {
        acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
      }
      output[i] = acc;
    }

    // Checksum calculation
    total = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      int64_t acc = output[i];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}