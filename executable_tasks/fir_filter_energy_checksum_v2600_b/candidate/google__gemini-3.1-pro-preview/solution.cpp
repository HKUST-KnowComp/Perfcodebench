#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

/**
 * Performance Engineer's Optimization:
 * 1. Loop Reordering: Changed the FIR calculation from sample-centric to tap-centric.
 *    This allows the inner loop to be a simple vector-scalar multiply-accumulate,
 *    which is highly amenable to compiler auto-vectorization and cache-friendly.
 * 2. Memory Management: Moved the 'output' vector allocation outside the iteration loop.
 * 3. Checksum Integration: The checksum is calculated in a separate pass after the FIR
 *    to maintain the exact logic while allowing the FIR part to be fully vectorized.
 * 4. Redundancy Elimination: Since 'total' is reset every iteration, only the last
 *    iteration is strictly necessary for the result. However, we keep the loop structure
 *    to ensure the benchmark measures the intended workload.
 */

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  
  if (iters <= 0) return 0;
  if (signal.empty()) return 1469598103934665603ULL;

  const size_t n = signal.size();
  const size_t m = taps.size();
  std::vector<int64_t> output(n);
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset output buffer for this iteration
    std::fill(output.begin(), output.end(), 0);

    // Tap-centric FIR calculation: O(N * M)
    // This structure allows the compiler to use SIMD (AVX2/SSE) for the inner loop.
    for (size_t j = 0; j < m; ++j) {
      const int64_t tap_val = static_cast<int64_t>(taps[j]);
      const int16_t* sig_ptr = &signal[0];
      int64_t* out_ptr = &output[j];
      
      // Only process where i >= j
      const size_t count = n - j;
      if (j < n) {
        for (size_t i = 0; i < count; ++i) {
          out_ptr[i] += static_cast<int64_t>(sig_ptr[i]) * tap_val;
        }
      }
    }

    // FNV-1a-like checksum calculation
    total = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < n; ++i) {
      int64_t acc = output[i];
      uint64_t energy = static_cast<uint64_t>(acc * acc + 3 * acc);
      total ^= energy;
      total *= prime;
    }
  }

  return total;
}