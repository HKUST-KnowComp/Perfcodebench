#include "interface.h"
#include <fast_float/fast_float.h>
#include <algorithm>
#include <string>

double parse_sum(const std::string& input) {
  // Single mutable buffer to avoid per-line heap allocations
  std::string buf = input;
  // Replace all Fortran-style d/D exponents once, not per token
  std::replace(buf.begin(), buf.end(), 'd', 'e');
  std::replace(buf.begin(), buf.end(), 'D', 'E');
  
  const char* p = buf.data();
  const char* end = p + buf.size();
  double sum = 0.0;
  
  while (p < end) {
    // Locate end of current newline-delimited line
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    
    // Fast, accurate parsing with fast_float
    double value = 0.0;
    auto parse_result = fast_float::from_chars(p, line_end, value);
    if (parse_result.ec == std::errc()) {
      sum += value;
    }
    
    // Advance to next line
    p = (line_end < end) ? line_end + 1 : line_end;
  }
  return sum;
}