#include "interface.h"
#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value;
    auto result = fast_float::from_chars(p, end, value, fast_float::chars_format::general);
    if (result.ec != std::errc()) {
      break;
    }
    sum += value;
    p = (result.ptr && *result.ptr == '
') ? result.ptr + 1 : result.ptr;
  }
  return sum;
}