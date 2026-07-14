#include "interface.h"

#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    double value;
    auto res = fast_float::from_chars(p, end, value, fast_float::chars_format::general);
    if (res.ec != std::errc()) {
      break; // parsing failed or no more numbers
    }
    sum += value;
    p = res.ptr;
    if (p < end && *p == '|') {
      ++p; // skip delimiter
    }
  }
  return sum;
}
