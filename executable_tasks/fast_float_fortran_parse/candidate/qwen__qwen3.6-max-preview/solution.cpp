#include "interface.h"

#include <fast_float/fast_float.h>
#include <string>
#include <cstring>
#include <system_error>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  char buf[256];

  while (p < end) {
    // Skip leading whitespace/newlines
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) ++p;
    if (p >= end) break;

    const char* start = p;
    while (p < end && *p != '\n') ++p;
    const char* line_end = p;

    size_t len = line_end - start;
    if (len >= sizeof(buf)) len = sizeof(buf) - 1;

    std::memcpy(buf, start, len);
    for (size_t i = 0; i < len; ++i) {
      char c = buf[i];
      if (c == 'd' || c == 'D') buf[i] = 'e';
    }

    double val;
    auto res = fast_float::from_chars(buf, buf + len, val);
    if (res.ec == std::errc()) {
      sum += val;
    }

    if (p < end) ++p;
  }
  return sum;
}