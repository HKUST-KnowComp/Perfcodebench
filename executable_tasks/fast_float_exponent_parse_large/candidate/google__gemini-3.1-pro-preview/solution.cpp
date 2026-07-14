#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    // Skip leading whitespace and newlines
    while (p < end && static_cast<unsigned char>(*p) <= ' ') {
      ++p;
    }
    if (p == end) {
      break;
    }
    
    double value;
    auto result = fast_float::from_chars(p, end, value);
    if (result.ptr == p) {
      break;
    }
    sum += value;
    p = result.ptr;
  }
  return sum;
}
