#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip common whitespace (space, tab, CR, LF)
    while (p < end) {
      unsigned char c = static_cast<unsigned char>(*p);
      if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        ++p;
      } else {
        break;
      }
    }
    if (p >= end) break;

    double value;
    auto r = fast_float::from_chars(p, end, value, fast_float::chars_format::general);

    // If nothing was parsed, stop (mirrors baseline behavior on invalid input)
    if (r.ptr == p) {
      break;
    }

    // Accept value even if ec indicates range issues, similar to strtod behavior
    sum += value;
    p = r.ptr;

    // Fast-path consume typical line endings
    if (p < end) {
      if (*p == '\n') {
        ++p;
      } else if (*p == '\r') {
        ++p;
        if (p < end && *p == '\n') ++p;
      }
    }
  }

  return sum;
}
