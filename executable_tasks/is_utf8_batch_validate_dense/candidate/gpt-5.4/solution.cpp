#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    count += is_utf8(s) ? 1u : 0u;
  }
  return count;
}
