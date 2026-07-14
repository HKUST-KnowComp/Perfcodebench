#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value = 0.0;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ptr == p) {
      break;
    }
    sum += value;
    p = (*res.ptr == '\n') ? res.ptr + 1 : res.ptr;
  }
  return sum;
}