#include "interface.h"
#include <simdjson.h>
#include <cstddef>
#include <string>
#include <vector>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (simdjson::validate_utf8(reinterpret_cast<const uint8_t*>(s.data()), s.size()) == 0) {
      count++;
    }
  }
  return count;
}