#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < codes.size(); ++i) {
    const std::string& s = dictionary[codes[i]];
    if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
      sum += weights[i];
    }
  }
  return sum;
}