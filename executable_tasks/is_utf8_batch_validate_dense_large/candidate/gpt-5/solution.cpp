#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  const std::size_t n = inputs.size();

  std::size_t i = 0;
  for (; i + 3 < n; i += 4) {
    const std::string &s0 = inputs[i];
    const std::string &s1 = inputs[i + 1];
    const std::string &s2 = inputs[i + 2];
    const std::string &s3 = inputs[i + 3];

    count += static_cast<std::size_t>(s0.empty() || is_utf8(s0.data(), s0.size()));
    count += static_cast<std::size_t>(s1.empty() || is_utf8(s1.data(), s1.size()));
    count += static_cast<std::size_t>(s2.empty() || is_utf8(s2.data(), s2.size()));
    count += static_cast<std::size_t>(s3.empty() || is_utf8(s3.data(), s3.size()));
  }
  for (; i < n; ++i) {
    const std::string &s = inputs[i];
    count += static_cast<std::size_t>(s.empty() || is_utf8(s.data(), s.size()));
  }

  return count;
}
