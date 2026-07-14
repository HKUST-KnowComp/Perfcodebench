#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value;
    auto r = fast_float::from_chars(p, end, value, fast_float::chars_format::general);
    if (r.ec != std::errc()) {
      break;
    }
    sum += value;
    p = r.ptr;
    if (p < end && *p == '\n') {
      ++p;
    }
  }
  return sum;
}
