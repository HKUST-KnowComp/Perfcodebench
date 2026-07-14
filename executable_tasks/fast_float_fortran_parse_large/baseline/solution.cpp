#include "interface.h"

#include <algorithm>
#include <cstdlib>
#include <string>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    std::string token(p, line_end);
    std::replace(token.begin(), token.end(), 'd', 'e');
    std::replace(token.begin(), token.end(), 'D', 'E');
    char* next = nullptr;
    const double value = std::strtod(token.c_str(), &next);
    if (next == token.c_str()) {
      break;
    }
    sum += value;
    p = (line_end < end) ? line_end + 1 : line_end;
  }
  return sum;
}
