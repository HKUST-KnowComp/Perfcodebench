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
      p = line_end + 1;
      continue;
    }
    
    const char* parse_ptr = p;
    bool needs_conversion = false;
    
    for (size_t i = 0; i < len; ++i) {
      if (p[i] == 'd' || p[i] == 'D') {
        needs_conversion = true;
        break;
      }
    }
    
    if (needs_conversion) {
      if (len < sizeof(buffer)) {
        memcpy(buffer, p, len);
        for (size_t i = 0; i < len; ++i) {
          if (buffer[i] == 'd') buffer[i] = 'e';
          else if (buffer[i] == 'D') buffer[i] = 'E';
        }
        parse_ptr = buffer;
      } else {
        p = (line_end < end) ? line_end + 1 : line_end;
        continue;
      }
    }
    
    double value;
    auto result = fast_float::from_chars(parse_ptr, parse_ptr + len, value);
    
    if (result.ec == std::errc()) {
      sum += value;
    }
    
    p = (line_end < end) ? line_end + 1 : line_end;
  }
  
  return sum;
}