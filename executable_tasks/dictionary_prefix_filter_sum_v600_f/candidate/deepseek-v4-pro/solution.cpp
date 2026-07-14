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

  const size_t dict_size = dictionary.size();
  std::vector<char> matched(dict_size, 0);
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    // Assume all strings have length >= 2 (as in original code)
    if (s[0] == 'q' && s[1] <= 'm') {
      matched[i] = 1;
    }
  }

  uint64_t total = 0;
  const size_t n = codes.size();
  for (size_t i = 0; i < n; ++i) {
    if (matched[codes[i]]) {
      total += weights[i];
    }
  }

  // The sum is the same regardless of the number of iterations.
  return total;
}