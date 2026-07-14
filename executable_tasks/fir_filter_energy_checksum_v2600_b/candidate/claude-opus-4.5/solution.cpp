#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const std::size_t sig_size = signal.size();
  const std::size_t tap_size = taps.size();
  
  if (sig_size == 0 || tap_size == 0) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
      total = 1469598103934665603ULL;
    }
    return total;
  }
  
  std::vector<int64_t> output(sig_size);
  const int16_t* __restrict sig_ptr = signal.data();
  const int16_t* __restrict tap_ptr = taps.data();
  int64_t* __restrict out_ptr = output.data();
  
  uint64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Phase 1: partial overlap (i < tap_size)
    const std::size_t phase1_end = (tap_size < sig_size) ? tap_size : sig_size;
    for (std::size_t i = 0; i < phase1_end; ++i) {
      int64_t acc = 0;
      const std::size_t limit = i + 1;
      const int16_t* s = sig_ptr + i;
      std::size_t j = 0;
      // Unroll by 4
      for (; j + 4 <= limit; j += 4) {
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j)]) * static_cast<int64_t>(tap_ptr[j]);
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j+1)]) * static_cast<int64_t>(tap_ptr[j+1]);
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j+2)]) * static_cast<int64_t>(tap_ptr[j+2]);
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j+3)]) * static_cast<int64_t>(tap_ptr[j+3]);
      }
      for (; j < limit; ++j) {
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j)]) * static_cast<int64_t>(tap_ptr[j]);
      }
      out_ptr[i] = acc;
    }
    
    // Phase 2: full overlap (i >= tap_size)
    for (std::size_t i = phase1_end; i < sig_size; ++i) {
      int64_t acc = 0;
      const int16_t* s = sig_ptr + i;
      std::size_t j = 0;
      // Unroll by 4
      for (; j + 4 <= tap_size; j += 4) {
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j)]) * static_cast<int64_t>(tap_ptr[j]);
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j+1)]) * static_cast<int64_t>(tap_ptr[j+1]);
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j+2)]) * static_cast<int64_t>(tap_ptr[j+2]);
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j+3)]) * static_cast<int64_t>(tap_ptr[j+3]);
      }
      for (; j < tap_size; ++j) {
        acc += static_cast<int64_t>(s[-static_cast<std::ptrdiff_t>(j)]) * static_cast<int64_t>(tap_ptr[j]);
      }
      out_ptr[i] = acc;
    }
    
    // Compute checksum
    total = 1469598103934665603ULL;
    for (std::size_t i = 0; i < sig_size; ++i) {
      int64_t acc = out_ptr[i];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  
  return total;
}