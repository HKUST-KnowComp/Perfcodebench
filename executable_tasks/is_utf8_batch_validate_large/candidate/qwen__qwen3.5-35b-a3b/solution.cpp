#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (is_utf8::validate(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}