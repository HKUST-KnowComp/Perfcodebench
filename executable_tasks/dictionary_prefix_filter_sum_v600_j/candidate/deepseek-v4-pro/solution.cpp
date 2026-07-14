#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  // Precompute which dictionary entries satisfy the prefix rule
  std::vector<uint8_t> match(dictionary.size(), 0);
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    if (s.length() >= 2 && s[0] == 'q' && s[1] <= 'm') {
      match[i] = 1;
    }
  }

  // Sum weights of codes that map to a matching dictionary entry
  uint64_t sum = 0;
  for (std::size_t i = 0; i < codes.size(); ++i) {
    if (match[codes[i]]) {
      sum += static_cast<uint64_t>(weights[i]);
    }
  }

  return sum;
}