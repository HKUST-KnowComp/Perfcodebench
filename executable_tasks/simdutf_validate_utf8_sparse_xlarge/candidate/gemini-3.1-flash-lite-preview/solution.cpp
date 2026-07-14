#include "interface.h"
#include "simdutf.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  simdutf::implementation* impl = simdutf::get_available_implementations().detect_best();
  
  for (const auto& s : inputs) {
    if (impl->validate_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}