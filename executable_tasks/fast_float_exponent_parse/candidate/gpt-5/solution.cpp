#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (true) {
    // Skip common whitespace delimiters (including newlines)
    while (p < end) {
      char c = *p;
      if (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
        ++p;
      } else {
        break;
      }
    }
    if (p >= end) break;

    double value;
    auto r = fast_float::from_chars(p, end, value);
    if (r.ec != std::errc()) {
      // Mimic baseline behavior: stop on invalid parse
      break;
    }
    sum += value;
    p = r.ptr;
  }

  return sum;
}
