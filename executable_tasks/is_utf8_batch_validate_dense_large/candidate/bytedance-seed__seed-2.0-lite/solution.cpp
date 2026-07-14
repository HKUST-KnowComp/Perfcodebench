#include "is_utf8.h"
#include <vector>
#include <string>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t valid_count = 0;
  for (const auto& str : inputs) {
    if (is_valid_utf8(str.data(), str.size())) {
      ++valid_count;
    }
  }
  return valid_count;
}