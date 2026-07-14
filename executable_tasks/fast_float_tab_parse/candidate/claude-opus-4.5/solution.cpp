#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  
  double sum = 0.0;
  double c = 0.0;  // Kahan summation compensation
  
  while (p < end) {
    // Skip any leading whitespace or empty fields
    while (p < end && *p == '\t') {
      ++p;
    }
    if (p >= end) break;
    
    double value;
    auto result = fast_float::from_chars(p, end, value);
    
    if (result.ec != std::errc() || result.ptr == p) {
      // Skip to next tab or end on parse failure
      while (p < end && *p != '\t') ++p;
      if (p < end) ++p;
      continue;
    }
    
    // Kahan summation for accuracy
    double y = value - c;
    double t = sum + y;
    c = (t - sum) - y;
    sum = t;
    
    p = result.ptr;
    // Skip the tab delimiter if present
    if (p < end && *p == '\t') {
      ++p;
    }
  }
  
  return sum;
}