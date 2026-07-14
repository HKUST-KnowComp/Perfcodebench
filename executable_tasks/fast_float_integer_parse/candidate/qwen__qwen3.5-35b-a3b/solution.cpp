#pragma once

#include <string>
#include <fast_float/fast_float.h>

double parse_sum(const std::string& input) {
  const char* p = input.c_str();
  const char* end = p + input.size();
  double sum = 0.0;
  
  while (p < end) {
    const char* next = p;
    auto result = fast_float::parse<double>(p, end, &next);
    
    if (result.ec != fast_float::errc::none || next == p) {
      break;
    }
    
    sum += result.value;
    
    // Skip to next line
    while (next < end && *next != '\n') {
      next++;
    }
    if (next < end && *next == '\n') {
      next++;
    }
    p = next;
  }
  
  return sum;
}