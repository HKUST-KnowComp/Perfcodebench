#include "interface.h"

#include "fast_float/fast_float.h"

#include <string>

// Fast path specialized for newline-delimited integer tokens.
// We parse directly from the character buffer, avoiding strtod and
// minimizing per-token overhead. The task guarantees integer tokens,
// so a simple digit scanner is sufficient and much faster.

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

    unsigned long long value = 0;
    while (p < end) {
      const unsigned char c = static_cast<unsigned char>(*p);
      const unsigned digit = static_cast<unsigned>(c - '0');
      if (digit > 9) break;
      value = value * 10 + digit;
      ++p;
    }

    sum += neg ? -static_cast<double>(value) : static_cast<double>(value);

    if (p < end && *p == '\n') {
      ++p;
    }
  }

  return sum;
}
