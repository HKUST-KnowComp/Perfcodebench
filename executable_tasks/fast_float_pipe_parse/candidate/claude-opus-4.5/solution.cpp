#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;
  double value;
  
  while (p < end) {
    auto result = fast_float::from_chars(p, end, value);
    if (__builtin_expect(result.ec != std::errc(), 0)) {
      // Skip invalid character and continue
      ++p;
      continue;
    }
    sum += value;
    p = result.ptr;
    // Skip pipe delimiter if present
    if (__builtin_expect(p < end && *p == '|', 1)) {
      ++p;
    }
  }
  return sum;
}