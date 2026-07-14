#include "interface.h"

#include <is_utf8.h>
#include <string>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    const char* line_start = p;
    while (p < end && *p != '\n') {
      ++p;
    }
    
    const std::size_t line_len = p - line_start;
    if (is_utf8(line_start, line_len)) {
      ++count;
    }
    
    if (p < end) ++p;
  }
  
  return count;
}