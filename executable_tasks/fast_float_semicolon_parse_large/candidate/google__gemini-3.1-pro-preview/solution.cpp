#include "interface.h"
#include "fast_float/fast_float.h"
#include <system_error>

/**
 * Optimized implementation of parse_sum using the fast_float library.
 * fast_float::from_chars is significantly faster than std::strtod as it is 
 * locale-independent and uses advanced algorithms for float parsing.
 */
double parse_sum(const std::string& input) {
  const char* ptr = input.data();
  const char* const end = ptr + input.size();
  double total_sum = 0.0;
  double current_value = 0.0;

  while (ptr < end) {
    // fast_float::from_chars parses the double and returns a result containing
    // the pointer to the first character not matching the pattern.
    auto result = fast_float::from_chars(ptr, end, current_value);
    
    if (result.ec == std::errc()) {
      total_sum += current_value;
      ptr = result.ptr;
    } else {
      // If parsing fails (e.g. leading semicolon or invalid format), 
      // we must advance to avoid an infinite loop, though the task 
      // implies valid semicolon-delimited doubles.
      if (ptr < end && *ptr != ';') {
          // Skip invalid characters if any
          while (ptr < end && *ptr != ';') ++ptr;
      }
    }

    // Skip the semicolon delimiter if present
    if (ptr < end && *ptr == ';') {
      ptr++;
    }
  }

  return total_sum;
}