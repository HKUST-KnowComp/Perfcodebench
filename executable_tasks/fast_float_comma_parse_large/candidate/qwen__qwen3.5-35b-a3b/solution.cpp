#pragma once

#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value;
    auto result = fast_float::parse_double(p, end, value);
    if (result.ec != fast_float::parse_error::none) {
      break;
    }
    sum += value;
    p = result.ptr;
    if (p < end && *p == ',') {
      p++;
    }
  }
  return sum;
}