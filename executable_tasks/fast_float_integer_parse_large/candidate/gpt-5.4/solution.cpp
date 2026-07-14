#include "interface.h"

#include "fast_float/fast_float.h"

#include <cstdint>
#include <string>

// Fast path specialized for newline-delimited integer tokens.
// We parse directly from the character buffer, avoiding strtod overhead.
// The task guarantees integer tokens, and the result is accumulated in double.

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();

  double sum = 0.0;

  while (p < end) {
    bool neg = false;
    if (*p == '-') {
      neg = true;
      ++p;
    } else if (*p == '+') {
      ++p;
    }

    // Parse integer digits. Use 64-bit accumulation for the common fast path.
    // If no digits are present, fall back to fast_float to preserve behavior.
    if (p >= end || static_cast<unsigned>(*p - '0') > 9u) {
      double value = 0.0;
      auto res = fast_float::from_chars(p, end, value);
      if (res.ptr == p) break;
      sum += neg ? -value : value;
      p = res.ptr;
      if (p < end && *p == '\n') ++p;
      continue;
    }

    uint64_t v = 0;
    do {
      v = v * 10u + static_cast<unsigned>(*p - '0');
      ++p;
    } while (p < end && static_cast<unsigned>(*p - '0') <= 9u);

    sum += neg ? -static_cast<double>(v) : static_cast<double>(v);

    if (p < end && *p == '\n') {
      ++p;
    }
  }

  return sum;
}
