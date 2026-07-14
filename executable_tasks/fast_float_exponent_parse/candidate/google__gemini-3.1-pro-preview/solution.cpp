#include "interface.h"
#include <fast_float/fast_float.h>
#include <system_error>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double total_sum = 0.0;
  double val;

  while (p < end) {
    // Skip leading whitespace or newlines if any (though usually fast_float handles start)
    while (p < end && (*p == '\n' || *p == '\r' || *p == ' ')) {
      p++;
    }
    
    if (p >= end) break;

    // fast_float::from_chars is significantly faster than strtod or sscanf
    auto result = fast_float::from_chars(p, end, val);
    
    if (result.ec == std::errc()) {
      total_sum += val;
      p = result.ptr;
    } else {
      // If parsing fails, skip the current character to avoid infinite loops
      p++;
    }

    // Advance past the delimiter (newline)
    while (p < end && (*p == '\n' || *p == '\r' || *p == ' ')) {
      p++;
    }
  }

  return total_sum;
}