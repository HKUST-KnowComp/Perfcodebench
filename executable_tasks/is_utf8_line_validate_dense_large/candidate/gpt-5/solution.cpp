#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  std::size_t count = 0;

  while (p < end) {
    const void* nl = std::memchr(p, '\n', static_cast<std::size_t>(end - p));
    const char* line_end = nl ? static_cast<const char*>(nl) : end;
    const std::size_t len = static_cast<std::size_t>(line_end - p);

    if (is_utf8(reinterpret_cast<const unsigned char*>(p), len)) {
      ++count;
    }

    if (!nl) {
      break; // no more newlines; last line processed
    }
    p = line_end + 1; // move past the newline
  }

  return count;
}
