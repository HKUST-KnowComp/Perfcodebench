#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  while (p < end) {
    const size_t remaining = static_cast<size_t>(end - p);
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', remaining));
    if (!line_end) {
      line_end = end;
    }
    if (is_utf8(p, static_cast<size_t>(line_end - p))) {
      ++count;
    }
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  return count;
}