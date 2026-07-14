#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (is_utf8::is_valid(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
      count++;
    }
  }
  return count;
}