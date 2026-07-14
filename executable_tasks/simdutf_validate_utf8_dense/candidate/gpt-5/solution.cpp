#include "interface.h"
#include "simdutf.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  const simdutf::implementation* impl = simdutf::get_active_implementation();
  std::size_t count = 0;
  for (const std::string& s : inputs) {
    count += impl->validate_utf8(s.data(), s.size()) ? 1u : 0u;
  }
  return count;
}
