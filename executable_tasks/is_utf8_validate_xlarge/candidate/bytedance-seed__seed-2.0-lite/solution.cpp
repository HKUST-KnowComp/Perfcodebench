#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t valid_count = 0;
  for (const auto& s : inputs) {
    if (is_utf8_validate(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
      valid_count++;
    }
  }
  return valid_count;
}