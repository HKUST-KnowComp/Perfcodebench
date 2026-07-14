#include "interface.h"

#include <fast_float/fast_float.h>

static inline bool is_ascii_space(unsigned char c) {
  // ASCII whitespace: space (0x20) and \t..\r (0x09..0x0D)
  return (c == ' ') || (c >= '\t' && c <= '\r');
}

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip ASCII whitespace fast
    while (p < end && is_ascii_space(static_cast<unsigned char>(*p))) {
      ++p;
    }
    if (p >= end) break;

    double value = 0.0;
    auto r = fast_float::from_chars(p, end, value);
    if (r.ec == std::errc()) {
      sum += value;
      p = r.ptr;
    } else {
      // Fallback progress to avoid stalling on unexpected characters
      ++p;
    }
  }

  return sum;
}
