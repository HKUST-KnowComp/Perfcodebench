#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <vector>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;
  double value = 0.0;

  while (p < end) {
    // fast_float::from_chars is significantly faster than std::strtod
    // as it is locale-independent and optimized for modern CPU architectures.
    auto result = fast_float::from_chars(p, end, value);
    
    if (result.ec == std::errc()) {
      sum += value;
      p = result.ptr;
      
      // Skip the comma delimiter if present
      if (p < end && *p == ',') {
        ++p;
      }
    } else {
      // If parsing fails (e.g. trailing whitespace or unexpected chars), 
      // we increment to avoid infinite loops, though the task implies valid CSV.
      if (p < end) {
        ++p;
      } else {
        break;
      }
    }
  }

  return sum;
}