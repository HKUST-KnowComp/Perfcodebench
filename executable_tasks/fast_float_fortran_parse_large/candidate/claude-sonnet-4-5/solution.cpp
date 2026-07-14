#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstring>

double parse_sum(const std::string& input) {
  const char* p = input.data();
  const char* end = p + input.size();
  double sum = 0.0;
  
  char buffer[128];
  
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    
    size_t len = line_end - p;
    if (len == 0) {
      p = (line_end < end) ? line_end + 1 : line_end;
      continue;
    }
    
    if (len < sizeof(buffer)) {
      size_t i = 0;
      for (const char* src = p; src < line_end; ++src, ++i) {
        char c = *src;
        if (c == 'd' || c == 'D') {
          c = 'e';
        }
        buffer[i] = c;
      }
      
      double value;
      auto result = fast_float::from_chars(buffer, buffer + len, value);
      if (result.ec == std::errc()) {
        sum += value;
      }
    } else {
      char* large_buffer = new char[len];
      size_t i = 0;
      for (const char* src = p; src < line_end; ++src, ++i) {
        char c = *src;
        if (c == 'd' || c == 'D') {
          c = 'e';
        }
        large_buffer[i] = c;
      }
      
      double value;
      auto result = fast_float::from_chars(large_buffer, large_buffer + len, value);
      if (result.ec == std::errc()) {
        sum += value;
      }
      delete[] large_buffer;
    }
    
    p = (line_end < end) ? line_end + 1 : line_end;
  }
  
  return sum;
}