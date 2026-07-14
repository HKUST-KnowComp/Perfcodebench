#include "interface.h"

#include <fast_float/fast_float.h>

static inline bool is_space(unsigned char c) {
  return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\v';
}

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (true) {
    // Skip whitespace
    while (p < end && is_space(static_cast<unsigned char>(*p))) {
      ++p;
    }
    if (p >= end) break;

    double value;
    auto r = fast_float::from_chars(p, end, value);
    if (r.ec == std::errc() && r.ptr != p) {
      sum += value;
      p = r.ptr;
    } else {
      // Unrecognized character: advance by one to avoid stalling
      ++p;
    }
  }

  return sum;
}
