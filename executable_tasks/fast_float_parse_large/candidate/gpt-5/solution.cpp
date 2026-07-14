#include "interface.h"

#include <cstdlib>
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    // Skip common whitespace characters
    while (p < end) {
      char c = *p;
      if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\v') {
        ++p;
      } else {
        break;
      }
    }
    if (p >= end) break;

    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec == std::errc()) {
      sum += value;
      p = result.ptr;
      // Skip a single newline or CR/LF following the number
      if (p < end) {
        if (*p == '\n') {
          ++p;
        } else if (*p == '\r') {
          ++p;
          if (p < end && *p == '\n') ++p;
        }
      }
      continue;
    }

    // If invalid at current position, mimic baseline behavior: stop.
    if (result.ec == std::errc::invalid_argument) {
      break;
    }

    // Fallback for rare out_of_range: use strtod.
    char* next = nullptr;
    double fallback = std::strtod(p, &next);
    if (next == p) {
      break;
    }
    sum += fallback;
    p = next;
    if (p < end) {
      if (*p == '\n') {
        ++p;
      } else if (*p == '\r') {
        ++p;
        if (p < end && *p == '\n') ++p;
      }
    }
  }

  return sum;
}
