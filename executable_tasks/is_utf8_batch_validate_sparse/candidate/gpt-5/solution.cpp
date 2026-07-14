#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    const char* p = s.data();
    const std::size_t n = s.size();
    count += static_cast<std::size_t>(is_utf8(p, n));
  }
  return count;
}
