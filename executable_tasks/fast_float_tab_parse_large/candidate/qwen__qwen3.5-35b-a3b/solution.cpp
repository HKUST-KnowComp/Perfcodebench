#pragma once

#include <string>
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != fast_float::errc::success) {
      break;
    }
    sum += value;
    p = result.ptr;
    if (p < end && *p == '\t') {
      p++;
    }
  }
  return sum;
}