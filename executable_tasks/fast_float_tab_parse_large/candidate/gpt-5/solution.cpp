#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (true) {
    // Skip ASCII whitespace (to match strtod behavior for robustness)
    while (p < end) {
      unsigned char c = static_cast<unsigned char>(*p);
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v') {
        ++p;
      } else {
        break;
      }
    }
    if (p >= end) {
      break;
    }

    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc()) {
      // No valid number at current position or other parsing error: stop.
      break;
    }

    sum += value;
    p = result.ptr;

    // Consume a single tab delimiter if present
    if (p < end && *p == '\t') {
      ++p;
    }
  }

  return sum;
}
