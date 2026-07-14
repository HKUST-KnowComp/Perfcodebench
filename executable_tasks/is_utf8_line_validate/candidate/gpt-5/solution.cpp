#include "interface.h"
#include "is_utf8.h"

#include <cstring>
#include <string>

std::size_t count_valid_lines(const std::string& input) {
  const char* const data = input.data();
  const char* p = data;
  const char* const end = data + input.size();
  std::size_t count = 0;

  while (p < end) {
    const void* found = std::memchr(p, '\n', static_cast<std::size_t>(end - p));
    const char* q = found ? static_cast<const char*>(found) : end;
    const std::size_t len = static_cast<std::size_t>(q - p);
    if (is_utf8(p, len)) {
      ++count;
    }
    if (!found) break;
    p = q + 1;
  }

  return count;
}
