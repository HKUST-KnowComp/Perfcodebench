#include "interface.h"

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  long long sum = 0;
  while (p < end) {
    // Skip whitespace/newlines
    while (p < end && (*p == '\n' || *p == ' ' || *p == '\t' || *p == '\r')) {
      ++p;
    }
    if (p >= end) break;
    
    // Parse sign
    bool neg = false;
    if (*p == '-') {
      neg = true;
      ++p;
    } else if (*p == '+') {
      ++p;
    }
    
    // Parse digits
    long long val = 0;
    while (p < end && *p >= '0' && *p <= '9') {
      val = val * 10 + (*p - '0');
      ++p;
    }
    
    sum += neg ? -val : val;
  }
  return static_cast<double>(sum);
}