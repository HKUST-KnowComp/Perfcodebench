#include "interface.h"
#include <simdutf.h>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t valid_count = 0;
  for (const auto& input_str : inputs) {
    if (simdutf::validate_utf8(input_str.data(), input_str.size())) {
      valid_count += 1;
    }
  }
  return valid_count;
}