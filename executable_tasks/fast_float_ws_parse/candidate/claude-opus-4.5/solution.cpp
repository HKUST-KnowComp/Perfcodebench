#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstdint>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  double c = 0.0;  // Kahan summation compensation

  while (p < end) {
    // Skip whitespace manually
    while (p < end && ((unsigned char)*p <= ' ')) {
      ++p;
    }
    if (p >= end) break;

    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec == std::errc() && result.ptr != p) {
      // Kahan summation for accuracy
      double y = value - c;
      double t = sum + y;
      c = (t - sum) - y;
      sum = t;
      p = result.ptr;
    } else {
      ++p;
    }
  }
  return sum;
}
