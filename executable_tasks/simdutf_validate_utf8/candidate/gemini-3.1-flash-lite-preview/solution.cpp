#include "interface.h"
#include "simdutf.h"

/**
 * Optimized implementation using the simdutf library.
 * simdutf provides highly optimized, architecture-specific SIMD kernels
 * for UTF-8 validation, which significantly outperforms scalar implementations.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  // Use the default implementation provided by simdutf, which automatically
  // selects the best available SIMD instruction set for the host CPU.
  for (const auto& s : inputs) {
    if (simdutf::validate_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}