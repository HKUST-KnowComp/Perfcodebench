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
    auto r = fast_float::from_chars(p, end, value);
    if (r.ec != std::errc()) {
      // Fallback for cases not supported by fast_float (e.g., INF/NAN)
      char* next = nullptr;
      double v2 = std::strtod(p, &next);
      if (next == p) {
        break;
      }
      sum += v2;
      p = (next < end && *next == ':') ? next + 1 : next;
      continue;
    }
    sum += value;
    p = r.ptr;
    if (p < end && *p == ':') {
      ++p;
    }
  }

  return sum;
}
