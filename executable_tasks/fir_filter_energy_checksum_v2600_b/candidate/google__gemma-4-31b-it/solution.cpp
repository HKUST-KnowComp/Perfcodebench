#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const size_t sig_size = signal.size();
  const size_t tap_size = taps.size();
  const int16_t* sig_ptr = signal.data();
  const int16_t* tap_ptr = taps.data();
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;
    for (size_t i = 0; i < sig_size; ++i) {
      int64_t acc = 0;
      size_t max_j = (i < tap_size) ? i + 1 : tap_size;
      
      // Manual unrolling or compiler auto-vectorization target
      for (size_t j = 0; j < max_j; ++j) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
      }
      
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}