#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  std::size_t start = 0;
  while (start < input.size()) {
    std::size_t end = start;
    while (end < input.size() && input[end] != '\n') {
      ++end;
    }
    count += is_utf8(input.data() + start, end - start) ? 1U : 0U;
    start = end + 1;
  }
  return count;
}