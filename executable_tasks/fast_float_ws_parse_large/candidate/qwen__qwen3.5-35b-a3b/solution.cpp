#include "interface.h"
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    auto result = fast_float::parse<double>(p, end);
    if (result.ptr == p) {
      ++p;
      continue;
    }
    sum += result.value;
    p = result.ptr;
  }
  return sum;
}