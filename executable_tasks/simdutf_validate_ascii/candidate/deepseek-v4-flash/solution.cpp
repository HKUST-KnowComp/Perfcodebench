#include "interface.h"
#include <simdutf.h>

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (simdutf::validate_ascii(s.data(), s.size())) {
      ++count;
    }
  }
  return count;
}