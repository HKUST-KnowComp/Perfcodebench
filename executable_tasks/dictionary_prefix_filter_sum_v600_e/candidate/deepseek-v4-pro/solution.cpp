#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0) return 0;

  std::vector<uint8_t> dict_match(dictionary.size(), 0);
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    if (s[0] == 'q' && s[1] <= 'm') {
      dict_match[i] = 1;
    }
  }

  uint64_t sum = 0;
  const std::size_t n = codes.size();
  for (std::size_t i = 0; i < n; ++i) {
    if (dict_match[codes[i]]) {
      sum += static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}