#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  const std::size_t n = inputs.size();
  for (std::size_t i = 0; i < n; ++i) {
    const std::string &s = inputs[i];
#if defined(__GNUC__) || defined(__clang__)
    if (i + 1 < n) {
      __builtin_prefetch(inputs[i + 1].data(), 0, 1);
    }
#endif
    // Empty strings are valid UTF-8
    if (s.empty()) {
      ++count;
      continue;
    }
    count += is_utf8(s.data(), s.size()) ? 1u : 0u;
  }
  return count;
}
