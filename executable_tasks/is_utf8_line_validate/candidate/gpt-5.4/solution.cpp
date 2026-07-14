#include "interface.h"
#include "is_utf8.h"

#include <cstddef>
#include <string>

std::size_t count_valid_lines(const std::string& input) {
  const char* const data = input.data();
  const std::size_t n = input.size();

  std::size_t count = 0;
  std::size_t start = 0;

  while (start < n) {
    std::size_t end = start;
    while (end < n && data[end] != '\n') {
      ++end;
    }

    count += is_utf8(data + start, end - start) ? 1u : 0u;
    start = end + 1;
  }

  return count;
}
