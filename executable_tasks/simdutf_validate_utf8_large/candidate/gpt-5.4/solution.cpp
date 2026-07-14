#include "interface.h"
#include "simdutf.h"

#include <cstddef>
#include <string>
#include <vector>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;

  // Use simdutf's highly optimized UTF-8 validator directly on each string.
  // This workload counts valid strings, so a simple tight loop minimizes
  // overhead while delegating validation to the SIMD implementation.
  for (const std::string& s : inputs) {
    count += simdutf::validate_utf8(s.data(), s.size()) ? std::size_t{1} : std::size_t{0};
  }

  return count;
}
