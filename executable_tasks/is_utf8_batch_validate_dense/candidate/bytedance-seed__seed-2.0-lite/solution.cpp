#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  const std::string* next_str = nullptr;
  for (const auto& s : inputs) {
    if (next_str != nullptr && next_str <= inputs.data() + inputs.size() - 1) {
      __builtin_prefetch(next_str->data(), 0, 1);
    }
    if (is_utf8_valid(s.data(), s.size())) {
      ++count;
    }
    next_str = &s + 1;
  }
  return count;
}