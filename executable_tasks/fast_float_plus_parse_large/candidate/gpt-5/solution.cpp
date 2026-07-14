#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    double value;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ec != std::errc{}) {
      break; // stop on parse error or no progress
    }
    sum += value;
    p = res.ptr;
    if (p < end && *p == '\n') {
      ++p;
    }
  }

  return sum;
}
