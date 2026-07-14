#include "interface.h"

#include <fast_float/fast_float.h>
#include <string>

// Parse a colon-delimited buffer of decimal numbers and return their sum.
// Optimized for large inputs by:
//  - avoiding C locale-aware strtod
//  - using fast_float::from_chars on bounded [ptr,end) ranges
//  - scanning directly for ':' delimiters without temporary substrings
//  - unrolling accumulation to reduce dependency chains

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();

  double sum0 = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;
  unsigned lane = 0;

  while (p < end) {
    const char* q = p;
    while (q < end && *q != ':') {
      ++q;
    }

    double value;
    auto res = fast_float::from_chars(p, q, value);
    if (res.ec != std::errc() || res.ptr != q) {
      break;
    }

    switch (lane) {
      case 0: sum0 += value; break;
      case 1: sum1 += value; break;
      case 2: sum2 += value; break;
      default: sum3 += value; break;
    }
    lane = (lane + 1u) & 3u;

    p = (q < end) ? (q + 1) : q;
  }

  return (sum0 + sum1) + (sum2 + sum3);
}
