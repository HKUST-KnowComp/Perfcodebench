#include "interface.h"
#include "simdutf.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const std::string& s : inputs) {
    count += simdutf::validate_utf8(s.data(), s.size()) ? 1u : 0u;
  }
  return count;
}
