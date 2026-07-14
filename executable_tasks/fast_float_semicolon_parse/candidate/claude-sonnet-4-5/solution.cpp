#include "interface.h"

#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
  std::string_view view(input);
  const char* p = view.data();
  const char* end = p + view.size();
  
  double sum = 0.0;
  double compensation = 0.0;  // Kahan summation for numerical stability
  
  while (p < end) {
    double value = 0.0;
    auto result = fast_float::from_chars(p, end, value);
    
    if (result.ec != std::errc()) {
      break;
    }
    
    // Kahan summation algorithm
    double y = value - compensation;
    double t = sum + y;
    compensation = (t - sum) - y;
    sum = t;
    
    p = result.ptr;
    
    // Skip semicolon if present
    if (p < end && *p == ';') {
      ++p;
    }
  }
  
  return sum;
}
