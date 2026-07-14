#include "interface.h"
#include <cctype>
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip leading whitespace delimiters
    while (p < end && std::isspace(static_cast<unsigned char>(*p))) {
      ++p;
    }
    if (p >= end) break;

    double value;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ec == std::errc()) {
      sum += value;
      p = res.ptr;
    } else {
      // Skip invalid characters to match baseline behavior
      ++p;
    }
  }
  return sum;
}