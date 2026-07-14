#include "interface.h"
#include <cstring>
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  std::size_t count = 0;

  while (p < end) {
    const void* found = std::memchr(p, '\n', static_cast<std::size_t>(end - p));
    const char* line_end = found ? static_cast<const char*>(found) : end;
    const std::size_t len = static_cast<std::size_t>(line_end - p);

    // Empty lines are valid UTF-8 without needing a call.
    if (len == 0 || is_utf8(p, len)) {
      ++count;
    }

    p = (line_end < end) ? (line_end + 1) : end;
  }

  return count;
}
