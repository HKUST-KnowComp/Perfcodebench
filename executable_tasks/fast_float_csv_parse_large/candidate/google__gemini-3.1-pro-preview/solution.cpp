#include "interface.h"
#include "fast_float/fast_float.h"
#include <string>
#include <system_error>

/**
 * Optimized CSV parser using the fast_float library.
 * fast_float::from_chars is significantly faster than std::strtod as it is 
 * locale-independent and optimized for modern CPU architectures.
 */
double parse_sum(const std::string& input) {
  const char* ptr = input.data();
  const char* const end = ptr + input.size();
  double total_sum = 0.0;
  double current_value;

  while (ptr < end) {
    // fast_float::from_chars parses the double and returns a pointer to the first character 
    // not part of the number.
    auto result = fast_float::from_chars(ptr, end, current_value);
    
    if (result.ec == std::errc()) {
      total_sum += current_value;
      ptr = result.ptr;
    } else {
      // If parsing failed, we might be at a comma or whitespace.
      // In a standard CSV, we skip the character and try again.
      // However, fast_float usually stops exactly at the delimiter.
      if (ptr < end) {
        ptr++;
        continue;
      } else {
        break;
      }
    }

    // Skip the comma delimiter if present.
    // We check ptr < end to avoid out-of-bounds access.
    if (ptr < end && *ptr == ',') {
      ptr++;
    }
    
    // Optional: Skip whitespace if the CSV format allows spaces after commas.
    // Based on the task description 'comma-separated buffer', we assume minimal whitespace.
    while (ptr < end && (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n')) {
      ptr++;
    }
  }

  return total_sum;
}