#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstdint>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  int64_t sum = 0;
  
  while (p < end) {
    // Skip any leading whitespace/newlines
    while (p < end && (*p == '\n' || *p == ' ' || *p == '\r' || *p == '\t')) {
      ++p;
    }
    if (p >= end) break;
    
    int64_t value = 0;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ec != std::errc() || result.ptr == p) {
      break;
    }
    sum += value;
    p = result.ptr;
  }
  
  return static_cast<double>(sum);
}