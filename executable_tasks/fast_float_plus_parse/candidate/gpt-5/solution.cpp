#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    if (*p == '\n') {
      ++p;
      continue;
    }

    double value;
    auto r = fast_float::from_chars(p, end, value);
    if (r.ec != std::errc() || r.ptr == p) {
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
