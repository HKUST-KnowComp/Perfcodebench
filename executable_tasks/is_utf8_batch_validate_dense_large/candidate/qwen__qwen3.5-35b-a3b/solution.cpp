#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  return is_utf8::count_valid_strings(inputs);
}