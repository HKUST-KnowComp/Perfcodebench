#include "interface.h"

#include <string>
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }

    if (p != line_end) {
      double value;
      auto res = fast_float::from_chars(p, line_end, value, fast_float::chars_format::fortran);
      if (res.ec != std::errc()) {
        break;
      }
      sum += value;
    }

    p = (line_end < end) ? (line_end + 1) : line_end;
  }

  return sum;
}
