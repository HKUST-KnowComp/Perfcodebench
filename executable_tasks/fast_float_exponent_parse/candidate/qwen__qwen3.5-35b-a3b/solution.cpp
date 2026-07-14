#include "fast_float/fast_float.h"
#include <string>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    auto result = fast_float::parse_double(p, end);
    if (result.ptr == p) {
      break;
    }
    sum += result.value;
    p = result.ptr;
    if (p < end && *p == '\n') {
      p++;
    }
  }
  return sum;
}