#include "interface.h"

#include <fast_float/fast_float.h>

#include <cctype>
#include <string>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  double sum = 0.0;

  while (p < end) {
    while (p < end && static_cast<unsigned char>(*p) <= ' ') {
      ++p;
    }
    if (p >= end) break;

    double value;
    auto res = fast_float::from_chars(p, end, value);
    if (res.ec == std::errc()) {
      sum += value;
      p = res.ptr;
    } else {
      ++p;
    }
  }

  return sum;
}
