#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  std::vector<int64_t> output(signal.size());
  uint64_t total = 0;
  size_t tap_size = taps.size();
  size_t signal_size = signal.size();

  const int16_t* signal_ptr = signal.data();
  const int16_t* taps_ptr = taps.data();

  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < tap_size && i < signal_size; ++i) {
      int64_t acc = 0;
      for (size_t j = 0; j <= i; ++j) {
        acc += static_cast<int64_t>(signal_ptr[i - j]) * static_cast<int64_t>(taps_ptr[j]);
      }
      output[i] = acc;
    }
    for (size_t i = tap_size; i < signal_size; ++i) {
      int64_t acc = 0;
      for (size_t j = 0; j < tap_size; ++j) {
        acc += static_cast<int64_t>(signal_ptr[i - j]) * static_cast<int64_t>(taps_ptr[j]);
      }
      output[i] = acc;
    }

    total = 1469598103934665603ULL;
    for (int64_t acc : output) {
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}