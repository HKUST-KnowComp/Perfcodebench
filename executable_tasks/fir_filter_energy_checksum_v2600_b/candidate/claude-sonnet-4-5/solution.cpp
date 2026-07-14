#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  const std::size_t sig_size = signal.size();
  const std::size_t tap_size = taps.size();
  const int16_t* sig_ptr = signal.data();
  const int16_t* tap_ptr = taps.data();
  
  uint64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    total = 1469598103934665603ULL;
    
    for (std::size_t i = 0; i < sig_size; ++i) {
      int64_t acc = 0;
      const std::size_t limit = (i < tap_size) ? (i + 1) : tap_size;
      std::size_t j = 0;
      
      // Unroll by 4
      for (; j + 3 < limit; j += 4) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
        acc += static_cast<int64_t>(sig_ptr[i - j - 1]) * static_cast<int64_t>(tap_ptr[j + 1]);
        acc += static_cast<int64_t>(sig_ptr[i - j - 2]) * static_cast<int64_t>(tap_ptr[j + 2]);
        acc += static_cast<int64_t>(sig_ptr[i - j - 3]) * static_cast<int64_t>(tap_ptr[j + 3]);
      }
      
      // Handle remainder
      for (; j < limit; ++j) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
      }
      
      // Compute energy checksum inline
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  
  return total;
}