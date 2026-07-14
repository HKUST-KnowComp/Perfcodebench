#include "interface.h"
#include "simdutf.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  simdutf::implementation &impl = simdutf::get_active_implementation();
  for (const auto &s : inputs) {
    count += static_cast<std::size_t>(impl.validate_utf8(s.data(), s.size()));
  }
  return count;
}
