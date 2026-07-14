#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip ASCII whitespace similar to strtod behavior
    while (p < end) {
      char c = *p;
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        ++p;
      } else {
        break;
      }
    }
    if (p >= end) break;

    double value;
    auto r = fast_float::from_chars(p, end, value);
    if (r.ec != std::errc()) {
      break; // no valid number at p
    }
    sum += value;
    p = r.ptr;
    if (p < end && *p == '|') {
      ++p; // skip pipe delimiter
    }
  }

  return sum;
}
