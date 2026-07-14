#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    if (is_utf8(p, line_end - p)) {
      ++count;
    }
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  return count;
}