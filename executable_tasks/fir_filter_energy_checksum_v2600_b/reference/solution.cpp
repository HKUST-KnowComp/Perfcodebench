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
      const std::size_t max_j = (i + 1u < taps.size()) ? i : (taps.size() - 1u);
      const int16_t* sig = signal.data() + i;
      const int16_t* tap = taps.data();
      for (std::size_t j = 0; j <= max_j; ++j) {
        acc += static_cast<int64_t>(sig[-static_cast<std::ptrdiff_t>(j)]) *
               static_cast<int64_t>(tap[j]);
      }
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}
