#include "interface.h"
#include "is_utf8.h"

#include <cstddef>
#include <string>

std::size_t count_valid_lines(const std::string& input) {
  const char* const data = input.data();
  const std::size_t n = input.size();

  std::size_t count = 0;
  std::size_t line_start = 0;

  for (std::size_t i = 0; i < n; ++i) {
    if (data[i] == '\n') {
      const std::size_t len = i - line_start;
      count += is_utf8(data + line_start, len) ? 1u : 0u;
      line_start = i + 1;
    }
  }

  if (line_start < n) {
    count += is_utf8(data + line_start, n - line_start) ? 1u : 0u;
  }

  return count;
}
