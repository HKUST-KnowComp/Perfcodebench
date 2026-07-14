#include "interface.h"
#include "is_utf8.h"

#include <string>

std::size_t count_valid_lines(const std::string& input) {
  const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* const end = data + input.size();
  const unsigned char* line_start = data;
  std::size_t count = 0;

  while (line_start <= end) {
    const unsigned char* line_end = line_start;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    size_t len = line_end - line_start;
    // empty lines are valid UTF-8
    if (len == 0 || is_utf8(reinterpret_cast<const char*>(line_start), len)) {
      ++count;
    }
    line_start = line_end + 1;
    if (line_start > end) break;
  }
  return count;
}