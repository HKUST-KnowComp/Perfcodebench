#include "interface.h"
#include "simdutf.h"

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
  const simdutf::implementation* impl = simdutf::get_active_implementation();
  std::size_t count = 0;
  for (const std::string& s : inputs) {
    if (impl->validate_ascii(s.data(), s.size())) {
      ++count;
    }
  }
  return count;
}
