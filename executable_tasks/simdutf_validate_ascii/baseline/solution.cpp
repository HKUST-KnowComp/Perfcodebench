#include "interface.h"

std::size_t count_ascii_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    bool ok = true;
    for (unsigned char ch : s) {
      if (ch > 0x7F) {
        ok = false;
        break;
      }
    }
    count += ok ? 1U : 0U;
  }
  return count;
}
