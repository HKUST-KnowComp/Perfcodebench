#include "interface.h"

#include <cstddef>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  double c = 0.0; // Kahan compensation

  while (p < end) {
    // Skip leading newlines and trivial whitespace
    while (p < end) {
      unsigned char ch = static_cast<unsigned char>(*p);
      if (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') {
        ++p;
      } else {
        break;
      }
    }
    if (p >= end) break;

    bool neg = false;
    if (*p == '+' || *p == '-') {
      neg = (*p == '-');
      ++p;
    }

    // Parse consecutive digits
    double val = 0.0;
    bool had_digits = false;
    while (p < end) {
      unsigned char d = static_cast<unsigned char>(*p);
      if (d >= '0' && d <= '9') {
        had_digits = true;
        val = val * 10.0 + double(d - '0');
        ++p;
      } else {
        break;
      }
    }

    if (!had_digits) {
      // No valid number on this line; skip to next newline
      while (p < end && *p != '\n') ++p;
      if (p < end && *p == '\n') ++p;
      continue;
    }

    if (neg) val = -val;

    // Kahan summation for improved numerical stability
    double y = val - c;
    double t = sum + y;
    c = (t - sum) - y;
    sum = t;

    // Advance to start of next line
    if (p < end) {
      if (*p == '\n') {
        ++p;
      } else {
        while (p < end && *p != '\n') ++p;
        if (p < end) ++p;
      }
    }
  }

  return sum;
}
