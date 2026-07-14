#include "interface.h"

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

    double value;
    auto result = fast_float::from_chars(p, line_end, value);
    if (result.ec != std::errc()) {
      break;
    }
    sum += value;

    p = (line_end < end) ? (line_end + 1) : line_end;
  }

  return sum;
}
