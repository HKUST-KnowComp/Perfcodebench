#include "interface.h"
#include "fast_float/fast_float.h"
#include <system_error>

static inline bool is_delim_char(unsigned char c) {
  // Fast ASCII-only delimiter check: commas and common whitespace
  return c == ',' || c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip delimiters between values
    while (p < end && is_delim_char(static_cast<unsigned char>(*p))) {
      ++p;
    }
    if (p >= end) break;

    double value;
    auto r = fast_float::from_chars(p, end, value);
    if (r.ec == std::errc()) {
      sum += value;
      p = r.ptr;
      continue;
    }
    if (r.ec == std::errc::result_out_of_range) {
      // Accept parsed approximation and advance even on overflow indication
      sum += value;
      p = r.ptr;
      continue;
    }
    // If we cannot parse at current position (invalid_argument), stop like baseline
    break;
  }

  return sum;
}
