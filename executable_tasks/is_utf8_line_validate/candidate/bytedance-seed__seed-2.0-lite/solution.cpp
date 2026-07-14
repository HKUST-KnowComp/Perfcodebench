#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* data = input.data();
  const std::size_t input_size = input.size();
  std::size_t start = 0;

  while (start < input_size) {
    std::size_t end = start;
    while (end < input_size && input[end] != '\n') {
      ++end;
    }
    if (is_utf8(data + start, end - start)) {
      ++count;
    }
    start = end + 1;
  }
  return count;
}