#include "interface.h"
#include <string>
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  std::string buffer;
  buffer.reserve(input.size());

  while (p < end) {
    const char* line_start = p;
    while (p < end && *p != '\n') {
      ++p;
    }
    size_t line_len = p - line_start;
    if (line_len == 0) {
      if (p < end) ++p;
      continue;
    }

    buffer.resize(line_len);
    std::copy(line_start, p, buffer.begin());

    for (char& c : buffer) {
      if (c == 'd') c = 'e';
      else if (c == 'D') c = 'E';
    }

    auto result = fast_float::parse_double(buffer.data(), buffer.data() + buffer.size());
    if (result.second != fast_float::parse_error::none) {
      break;
    }
    sum += result.first;

    if (p < end) ++p;
  }

  return sum;
}