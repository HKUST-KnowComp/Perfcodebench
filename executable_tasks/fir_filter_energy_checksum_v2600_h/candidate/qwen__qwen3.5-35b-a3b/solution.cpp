#pragma once

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  std::vector<int64_t> output;
  output.reserve(signal.size());
  uint64_t total = 0;
  const size_t M = taps.size();
  const size_t N = signal.size();

  for (int iter = 0; iter < iters; ++iter) {
    output.clear();
    
    // Handle initial samples where i < M
    for (size_t i = 0; i < N && i < M; ++i) {
      int64_t acc = 0;
      for (size_t j = 0; j <= i; ++j) {
        acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
      }
      output.push_back(acc);
    }

    // Handle remaining samples with full tap window
    for (size_t i = M; i < N; ++i) {
      int64_t acc = 0;
      for (size_t j = 0; j < M; ++j) {
        acc += static_cast<int64_t>(signal[i - j]) * static_cast<int64_t>(taps[j]);
      }
      output.push_back(acc);
    }

    total = 1469598103934665603ULL;
    for (int64_t acc : output) {
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  return total;
}