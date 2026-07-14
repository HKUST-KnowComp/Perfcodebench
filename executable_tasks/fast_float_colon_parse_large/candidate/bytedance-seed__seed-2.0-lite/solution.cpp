#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = input.data() + input.size();
  double sum = 0.0;
  while (p < end) {
    double value;
    auto parse_result = fast_float::from_chars(p, end, value);
    if (parse_result.ec != std::errc()) {
      break;
    }
    const char* next = parse_result.ptr;
    sum += value;
    p = (*next == ':') ? next + 1 : next;
  }
  return sum;
}