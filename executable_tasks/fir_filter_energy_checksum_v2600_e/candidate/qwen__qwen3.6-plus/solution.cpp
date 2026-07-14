#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  if (iters <= 0) return 0;
  
  uint64_t total = 1469598103934665603ULL;
  const size_t n = signal.size();
  const size_t m = taps.size();
  const int16_t* sig = signal.data();
  const int16_t* tap = taps.data();
  
  for (size_t i = 0; i < n; ++i) {
    int64_t acc = 0;
    size_t limit = (i + 1 < m) ? i + 1 : m;
    for (size_t j = 0; j < limit; ++j) {
      acc += static_cast<int64_t>(sig[i - j]) * tap[j];
    }
    total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
    total *= 1099511628211ULL;
  }
  
  return total;
}