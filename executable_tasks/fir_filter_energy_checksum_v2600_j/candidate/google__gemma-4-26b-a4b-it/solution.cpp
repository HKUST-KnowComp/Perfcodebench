#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const size_t n = signal.size();
  const size_t m = taps.size();
  
  // Pre-convert taps to int64_t to avoid repeated casting in the inner loop
  std::vector<int64_t> taps64(m);
  for (size_t j = 0; j < m; ++j) {
    taps64[j] = static_cast<int64_t>(taps[j]);
  }

  // Pre-convert signal to int64_t to avoid repeated casting
  std::vector<int64_t> signal64(n);
  for (size_t i = 0; i < n; ++i) {
    signal64[i] = static_cast<int64_t>(signal[i]);
  }

  uint64_t total = 0;
  // Reusable buffer for output to avoid repeated allocations
  std::vector<int64_t> output(n);

  for (int iter = 0; iter < iters; ++iter) {
    // FIR Convolution
    for (size_t i = 0; i < n; ++i) {
      int64_t acc = 0;
      // Optimization: split the loop to remove the 'if (j <= i)' branch
      // Phase 1: j <= i (partial overlap)
      size_t limit = (i < m) ? (i + 1) : m;
      for (size_t j = 0; j < limit; ++j) {
        acc += signal64[i - j] * taps64[j];
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