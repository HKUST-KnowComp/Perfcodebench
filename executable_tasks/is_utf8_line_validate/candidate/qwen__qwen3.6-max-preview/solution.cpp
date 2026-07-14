#include "interface.h"
#include "is_utf8.h"
#include <string>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* data = input.data();
  const char* end = data + input.size();
  const char* start = data;
  
  while (start < end) {
    const char* nl = static_cast<const char*>(std::memchr(start, '\n', end - start));
    const char* line_end = nl ? nl : end;
    if (is_utf8(start, static_cast<std::size_t>(line_end - start))) {
      ++count;
    }
    start = line_end + 1;
  }
  return count;
}