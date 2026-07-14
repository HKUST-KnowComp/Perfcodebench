#include "interface.h"
#include "fast_float/fast_float.h"

#include <cstdlib>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value;
    const char* next = p;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc()) {
      break;
    }
    sum += value;
    p = result.ptr;
    if (p < end && *p == ':') {
      ++p;
    }
  }
  return sum;
}