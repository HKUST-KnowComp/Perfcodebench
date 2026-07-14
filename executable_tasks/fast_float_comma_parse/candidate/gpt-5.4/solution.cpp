#include "interface.h"

#include <fast_float/fast_float.h>

#include <string>

// Parse comma-delimited decimal numbers and return their sum.
// Optimized for throughput by using fast_float directly on bounded ranges,
// avoiding locale-dependent strtod and any temporary substring creation.
double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    double value;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ec != std::errc()) {
      break;
    }
    sum += value;
    p = res.ptr;
    if (p < end && *p == ',') {
      ++p;
    }
  }

  return sum;
}
