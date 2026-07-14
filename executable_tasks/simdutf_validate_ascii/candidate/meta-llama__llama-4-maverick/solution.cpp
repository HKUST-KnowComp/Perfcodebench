#include "interface.h"
#include "simdutf.h"

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    count += simdutf::valid_ascii(s.data(), s.size()) ? 1 : 0;
  }
  return count;
}