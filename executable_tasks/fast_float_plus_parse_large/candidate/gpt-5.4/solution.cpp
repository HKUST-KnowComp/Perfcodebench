#include "interface.h"

#include "fast_float/fast_float.h"

#include <string>

// Input is newline-delimited explicitly signed doubles.
// We parse each line with fast_float and accumulate the sum.
// This avoids the overhead of strtod and locale handling.
double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc()) {
      break;
    }
    sum += value;
    p = result.ptr;
    if (p < end && *p == '\n') {
      ++p;
    }
  }

  return sum;
}
