#include "interface.h"

#include <cstdlib>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  while (p < end) {
    char* next = nullptr;
    const double value = std::strtod(p, &next);
    if (next == p) {
      break;
    }
    sum += value;
    p = (*next == '\n') ? next + 1 : next;
  }
  return sum;
}
