#include "interface.h"

#include <fast_float/fast_float.h>

static inline bool is_ascii_space(unsigned char c) {
  return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\v';
}

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();

  double sum = 0.0;
  double comp = 0.0; // Kahan compensation

  while (p < end) {
    // Skip leading ASCII whitespace (to match strtod behavior)
    while (p < end && is_ascii_space(static_cast<unsigned char>(*p))) {
      ++p;
    }
    if (p >= end) break;

    double value = 0.0;
    auto r = fast_float::from_chars(p, end, value, fast_float::chars_format::general);
    if (r.ec != std::errc()) {
      // Could not parse a number at current position; stop to mirror baseline behavior
      break;
    }

    // Kahan summation: sum += value
    double y = value - comp;
    double t = sum + y;
    comp = (t - sum) - y;
    sum = t;

    p = r.ptr;
    // Consume a single comma delimiter if present (baseline only consumed one)
    if (p < end && *p == ',') {
      ++p;
    }
  }

  return sum;
}
