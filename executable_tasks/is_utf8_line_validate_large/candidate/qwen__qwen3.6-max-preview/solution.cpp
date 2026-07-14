#include "interface.h"
#include "is_utf8.h"
#include <string>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* data = input.data();
  std::size_t len = input.size();
  std::size_t start = 0;
  
  while (start < len) {
    const char* current = data + start;
    std::size_t remaining = len - start;
    const char* nl = static_cast<const char*>(std::memchr(current, '\n', remaining));
    std::size_t end = nl ? static_cast<std::size_t>(nl - data) : len;
    
    if (is_utf8(current, end - start)) {
      ++count;
    }
    start = end + 1;
  }
  return count;
}
