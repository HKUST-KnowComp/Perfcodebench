#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  
  // Process 4 values at a time for better ILP
  while (p + 16 < end) {  // Conservative bound for unrolling
    double v0 = 0.0, v1 = 0.0, v2 = 0.0, v3 = 0.0;
    
    auto r0 = fast_float::from_chars(p, end, v0);
    if (r0.ec != std::errc()) break;
    p = r0.ptr;
    if (p < end && *p == '\n') ++p;
    
    auto r1 = fast_float::from_chars(p, end, v1);
    if (r1.ec != std::errc()) {
      sum += v0;
      break;
    }
    p = r1.ptr;
    if (p < end && *p == '\n') ++p;
    
    auto r2 = fast_float::from_chars(p, end, v2);
    if (r2.ec != std::errc()) {
      sum += v0 + v1;
      break;
    }
    p = r2.ptr;
    if (p < end && *p == '\n') ++p;
    
    auto r3 = fast_float::from_chars(p, end, v3);
    if (r3.ec != std::errc()) {
      sum += v0 + v1 + v2;
      break;
    }
    p = r3.ptr;
    if (p < end && *p == '\n') ++p;
    
    sum += v0 + v1 + v2 + v3;
  }
  
  // Handle remaining values
  while (p < end) {
    double value = 0.0;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc()) break;
    sum += value;
    p = result.ptr;
    if (p < end && *p == '\n') ++p;
  }
  
  return sum;
}