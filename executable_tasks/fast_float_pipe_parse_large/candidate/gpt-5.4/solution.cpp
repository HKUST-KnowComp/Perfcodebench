#include "interface.h"

#include "fast_float/fast_float.h"

#include <string>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    const char* q = p;
    while (q < end && *q != '|') {
      ++q;
    }

    double value;
    auto result = fast_float::from_chars(p, q, value);
    if (result.ec != std::errc()) {
      break;
    }
    sum += value;

    p = (q < end) ? (q + 1) : q;
  }

  return sum;
}
