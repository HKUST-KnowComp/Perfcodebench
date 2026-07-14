#include "interface.h"
#include "simdutf.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  // simdutf::validate_utf8 is highly optimized for modern CPUs using SIMD.
  // It is the standard way to perform this check efficiently.
  for (const auto& s : inputs) {
    if (simdutf::validate_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}