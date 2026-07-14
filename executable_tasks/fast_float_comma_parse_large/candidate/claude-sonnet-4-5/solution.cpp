#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  
  while (p < end) {
    // Skip any leading whitespace
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
      ++p;
    }
    
    if (p >= end) break;
    
    // Find the next comma or end
    const char* comma = p;
    while (comma < end && *comma != ',') {
      ++comma;
    }
    
    // Parse the number using fast_float
    double value = 0.0;
    auto result = fast_float::from_chars(p, comma, value);
    
    if (result.ec == std::errc()) {
      sum += value;
    }
    
    // Move past the comma
    p = (comma < end) ? comma + 1 : comma;
  }
  
  return sum;
}
