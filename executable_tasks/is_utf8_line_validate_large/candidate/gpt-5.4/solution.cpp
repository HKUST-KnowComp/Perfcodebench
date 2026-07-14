#include "interface.h"
#include "is_utf8.h"

#include <cstddef>
#include <string>

std::size_t count_valid_lines(const std::string& input) {
  const char* const data = input.data();
  const std::size_t n = input.size();

  std::size_t count = 0;
  std::size_t start = 0;

  for (std::size_t i = 0; i < n; ++i) {
    if (data[i] == '\n') {
      count += is_utf8(data + start, i - start) ? 1u : 0u;
      start = i + 1;
    }
  }

  if (start < n) {
    count += is_utf8(data + start, n - start) ? 1u : 0u;
  }

  return count;
}
