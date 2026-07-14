#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  std::size_t count = 0;

  while (p < end) {
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', static_cast<size_t>(end - p)));
    if (!line_end) {
      line_end = end;
    }
    const std::size_t len = static_cast<std::size_t>(line_end - p);
    if (len == 0) {
      // Empty line is valid UTF-8
      ++count;
    } else {
      // Use fast UTF-8 validator from is_utf8.h
      if (is_utf8(p, len)) {
        ++count;
      }
    }
    p = (line_end == end) ? end : (line_end + 1);
  }

  return count;
}
