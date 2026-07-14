#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;
    for (std::size_t i = 0; i < signal.size(); ++i) {
      int64_t acc = 0;
      for (std::size_t j = 0; j < taps.size() && j <= i; ++j) {
        acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
      }
      int64_t acc_squared = acc * acc + 3 * acc;
      total ^= static_cast<uint64_t>(acc_squared);
      total *= 1099511628211ULL;
    }
  }
  return total;
}