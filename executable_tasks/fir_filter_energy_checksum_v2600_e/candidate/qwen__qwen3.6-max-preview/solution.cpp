#include "interface.h"

#include <cstdint>
#include <cstddef>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  size_t N = signal.size();
  size_t M = taps.size();
  std::vector<int64_t> output(N, 0);

  const int16_t* sig_ptr = signal.data();
  int64_t* out_ptr = output.data();

  // Loop interchange: transforms O(N*M) backward/random access into sequential forward access.
  // This removes branches, improves cache locality, and allows full auto-vectorization.
  for (size_t j = 0; j < M && j < N; ++j) {
    int64_t t = taps[j];
    for (size_t i = 0; i < N - j; ++i) {
      out_ptr[i + j] += static_cast<int64_t>(sig_ptr[i]) * t;
    }
  }

  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;
    for (size_t i = 0; i < N; ++i) {
      int64_t acc = out_ptr[i];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}