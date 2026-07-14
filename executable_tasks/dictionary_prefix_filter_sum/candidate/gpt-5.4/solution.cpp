#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  std::vector<uint8_t> matches(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    matches[i] = static_cast<uint8_t>(s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
  }

  uint64_t sum = 0;
  const std::size_t n = codes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t code = static_cast<std::size_t>(codes[i]);
      local_sum += static_cast<uint64_t>(weights[i]) * static_cast<uint64_t>(matches[code]);
    }
    sum = local_sum;
  }

  return sum;
}
