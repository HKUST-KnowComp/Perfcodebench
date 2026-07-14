#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip line breaks (handles both \n and \r)
    while (p < end && (*p == '\n' || *p == '\r')) {
      ++p;
    }
    if (p >= end) break;

    double value;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ec != std::errc()) {
      // No valid number at current position; stop to avoid infinite loop
      break;
    }
    sum += value;
    p = res.ptr;
  }

  return sum;
}
