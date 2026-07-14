#include "interface.h"

#include <string>
#include <algorithm>
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  std::string buffer = input;
  std::replace(buffer.begin(), buffer.end(), 'd', 'e');
  std::replace(buffer.begin(), buffer.end(), 'D', 'E');
  
  const char* p = buffer.c_str();
  const char* end = p + buffer.size();
  double sum = 0.0;
  
  while (p < end) {
    const char* line_start = p;
    while (p < end && *p != '\n') {
      ++p;
    }
    const char* line_end = p;
    
    auto result = fast_float::parse_double(line_start, line_end);
    if (result.ec != fast_float::errc::success) {
      break;
    }
    sum += result.value;
    
    if (p < end) {
      ++p; // Skip the newline character
    }
  }
  
  return sum;
}