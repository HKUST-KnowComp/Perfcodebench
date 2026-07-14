#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstdlib>
#include <cerrno>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    double value;
    auto r = fast_float::from_chars(p, end, value, fast_float::chars_format::general);

    if (r.ec == std::errc()) {
      sum += value;
      p = r.ptr;
      if (p < end) {
        if (*p == '\t') {
          ++p;
        } else {
          break; // only tab-delimited; stop on any other character
        }
      }
      continue;
    }

    // If parsing did not advance, mirror baseline behavior: stop parsing.
    if (r.ptr == p) {
      break;
    }

    // Rare fallback for out_of_range or other errors after advancing.
    char* next = nullptr;
    errno = 0;
    double v = std::strtod(p, &next);
    if (next == p) {
      break;
    }
    sum += v;
    p = (next < end && *next == '\t') ? next + 1 : next;
  }

  return sum;
}
