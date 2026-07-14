#include "interface.h"

#include <fast_float/fast_float.h>

#include <string>

// Parse a comma-separated list of scientific-notation doubles and return the sum.
// Optimized for throughput by using fast_float directly on the input buffer,
// avoiding locale-aware strtod overhead and any temporary allocations.
double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc()) {
      break;
    }

    sum += value;
    p = result.ptr;

    if (p < end && *p == ',') {
      ++p;
    }
  }

  return sum;
}
