#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstdlib>
#include <system_error>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    double value;
    auto r = fast_float::from_chars(p, end, value, fast_float::chars_format::general);
    if (r.ec == std::errc{}) {
      sum += value;
      p = r.ptr;
      if (p < end && *p == ';') {
        ++p;
      }
      continue;
    }

    // Fallback to std::strtod to emulate edge behaviors (e.g., overflow to inf)
    char* next = nullptr;
    double v2 = std::strtod(p, &next);
    if (next == p) {
      break; // No progress, stop like the baseline
    }
    sum += v2;
    p = (next < end && *next == ';') ? next + 1 : next;
  }

  return sum;
}
