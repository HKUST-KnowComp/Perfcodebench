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
  
  // Pre-allocate buffer to avoid repeated allocations
  std::vector<int64_t> output(sig_size);

  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < sig_size; ++i) {
      int64_t acc = 0;
      size_t max_j = (i < tap_size) ? i + 1 : tap_size;
      
      // Manual unrolling for the FIR dot product
      size_t j = 0;
      for (; j + 3 < max_j; j += 4) {
        acc += (int64_t)sig_ptr[i - j] * tap_ptr[j];
        acc += (int64_t)sig_ptr[i - j - 1] * tap_ptr[j + 1];
        acc += (int64_t)sig_ptr[i - j - 2] * tap_ptr[j + 2];
        acc += (int64_t)sig_ptr[i - j - 3] * tap_ptr[j + 3];
      }
      for (; j < max_j; ++j) {
        acc += (int64_t)sig_ptr[i - j] * tap_ptr[j];
      }
      output[i] = acc;
    }

    // The baseline resets total every iteration
    total = 1469598103934665603ULL;
    for (size_t i = 0; i < sig_size; ++i) {
      int64_t acc = output[i];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}