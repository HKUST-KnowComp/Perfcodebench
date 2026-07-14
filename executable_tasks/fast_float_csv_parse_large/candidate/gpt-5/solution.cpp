#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip separators and common whitespace/newlines
    while (p < end) {
      char c = *p;
      if (c == ',' || c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        ++p;
      } else {
        break;
      }
    }
    if (p >= end) break;

    double value;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ec == std::errc::invalid_argument) {
      break; // no progress possible
    }
    // Accept parsed value even if out_of_range to mirror strtod behavior
    sum += value;
    p = res.ptr;
  }

  return sum;
}
