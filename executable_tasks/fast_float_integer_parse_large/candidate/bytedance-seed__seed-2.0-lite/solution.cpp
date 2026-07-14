#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value;
    auto parse_result = fast_float::from_chars(p, end, value);
    if (parse_result.ptr == p) {
      break;
    }
    sum += value;
    const char* next = parse_result.ptr;
    p = (*next == '\n') ? next + 1 : next;
  }
  return sum;
}