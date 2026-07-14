#include "interface.h"
#include "simdutf.h"

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const std::string& s : inputs) {
    count += simdutf::validate_ascii(s.data(), s.size()) ? 1u : 0u;
  }
  return count;
}
