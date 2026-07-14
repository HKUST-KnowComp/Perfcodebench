#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '
') {
      ++line_end;
    }
    count += is_utf8(p, static_cast<std::size_t>(line_end - p)) ? 1U : 0U;
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  return count;
}