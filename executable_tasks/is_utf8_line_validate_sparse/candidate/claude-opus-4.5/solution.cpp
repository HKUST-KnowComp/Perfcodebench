#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', static_cast<std::size_t>(end - p)));
    if (!line_end) {
      line_end = end;
    }
    std::size_t len = static_cast<std::size_t>(line_end - p);
    if (is_utf8(p, len)) {
      ++count;
    }
    p = (line_end < end) ? (line_end + 1) : end;
  }
  return count;
}
