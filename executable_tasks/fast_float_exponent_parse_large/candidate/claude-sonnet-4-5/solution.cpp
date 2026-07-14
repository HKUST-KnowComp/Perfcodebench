#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  double value;
  
  while (p < end) {
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