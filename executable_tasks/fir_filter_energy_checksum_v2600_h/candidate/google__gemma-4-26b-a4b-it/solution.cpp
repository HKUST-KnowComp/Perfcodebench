#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const size_t n = signal.size();
  const size_t m = taps.size();
  
  // Pre-allocate output to avoid reallocations
  std::vector<int64_t> output(n);
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Optimized FIR Convolution
    for (size_t i = 0; i < n; ++i) {
      int64_t acc = 0;
      // The condition j <= i is equivalent to j <= min(i, m-1)
      size_t limit = (i < m) ? (i + 1) : m;
      
      // Unrolling or compiler auto-vectorization works best with simple loops
      for (size_t j = 0; j < limit; ++j) {
        acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
      }
      output[i] = acc;
    }

    // The baseline has a hardcoded total reset per iteration
    total = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      int64_t acc = output[i];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}