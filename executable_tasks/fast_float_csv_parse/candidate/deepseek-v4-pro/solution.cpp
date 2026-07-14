#include "interface.h"
#include <fast_float/fast_float.h>
#include <system_error>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // skip leading whitespace (as strtod does)
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
      ++p;
    }
    if (p >= end) break;

    double value;
    auto [ptr, ec] = fast_float::from_chars(p, end, value);

    if (ec != std::errc()) {
      // parsing error or no digits
      if (ptr == p) break; // no progress
      // partial parse? advance to break anyway
      break;
    }

    sum += value;
    p = ptr;

    // If the next character is a comma, skip it.
    // Otherwise (e.g., whitespace or end), let the next iteration handle it
    // by skipping whitespace at the top of the loop.
    if (p < end && *p == ',') {
      ++p;
    }
  }

  return sum;
}
