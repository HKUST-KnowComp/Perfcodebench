#include "interface.h"
#include "is_utf8.h"
#include <string>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* const data = input.data();
  const std::size_t total_size = input.size();
  std::size_t start = 0;

  while (start < total_size) {
    const char* const newline_ptr = static_cast<const char*>(
      memchr(data + start, '\n', total_size - start)
    );
    const std::size_t end = newline_ptr ? (newline_ptr - data) : total_size;
    const std::size_t line_len = end - start;
    if (is_utf8(data + start, line_len)) {
      ++count;
    }
    start = end + 1;
  }
  return count;
}