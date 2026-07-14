#include "interface.h"
#include "is_utf8.h"
#include <string>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  std::size_t start = 0;
  std::size_t n = input.size();
  while (start < n) {
    std::size_t end = start;
    while (end < n && input[end] != '\n') {
      ++end;
    }
    if (is_utf8::is_valid(input.data() + start, end - start)) {
      ++count;
    }
    start = end + 1;
  }
  return count;
}