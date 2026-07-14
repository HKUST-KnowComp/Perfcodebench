#include "interface.h"
#include "simdutf.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // simdutf::validate_utf8 returns true if the string is valid UTF-8.
    // It is highly optimized using SIMD instructions.
    if (simdutf::validate_utf8(reinterpret_cast<const uint8_t*>(s.data()), s.size())) {
      count++;
    }
  }
  return count;
}