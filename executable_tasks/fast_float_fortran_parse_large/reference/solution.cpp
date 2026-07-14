#include "interface.h"

#include "fast_float/fast_float.h"

double parse_sum(const std::string& input) {
  const fast_float::parse_options options{
      fast_float::chars_format::fortran |
      fast_float::chars_format::allow_leading_plus};
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    double value = 0.0;
    auto result = fast_float::from_chars_advanced(p, end, value, options);
    if (result.ec != std::errc()) {
      break;
    }
    sum += value;
    p = (result.ptr < end && *result.ptr == '\n') ? result.ptr + 1 : result.ptr;
  }
  return sum;
}
