#include "interface.h"
#include "simdjson.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t valid_count = 0;
  for (const auto& input_str : inputs) {
    valid_count += simdjson::validate_utf8(input_str.data(), input_str.size()) ? 1U : 0U;
  }
  return valid_count;
}