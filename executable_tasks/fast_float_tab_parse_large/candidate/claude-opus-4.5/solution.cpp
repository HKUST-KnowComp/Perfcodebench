#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  double c = 0.0;  // Kahan summation compensation
  
  while (p < end) {
    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc() || result.ptr == p) {
      break;
    }
    // Kahan summation for accuracy
    double y = value - c;
    double t = sum + y;
    c = (t - sum) - y;
    sum = t;
    
    p = result.ptr;
    if (p < end && *p == '\t') {
      ++p;
    }
  }
  return sum;
}