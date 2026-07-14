#include "interface.h"
#include <fast_float/fast_float.h>
#include <cctype>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    while (p < end && std::isspace(static_cast<unsigned char>(*p))) {
      p++;
    }
    if (p >= end) break;

    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc()) {
      break;
    }
    sum += value;
    p = result.ptr;
    if (p < end && *p == '\n') {
      p++;
    }
  }
  return sum;
}