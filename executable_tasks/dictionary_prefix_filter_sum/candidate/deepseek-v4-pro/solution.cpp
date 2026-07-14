#include "interface.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
  return s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const std::size_t dict_size = dictionary.size();
  std::vector<char> match(dict_size, 0);
  for (std::size_t i = 0; i < dict_size; ++i) {
    if (matches_prefix(dictionary[i])) {
      match[i] = 1;
    }
  }

  const std::size_t n = codes.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (match[static_cast<std::size_t>(codes[i])]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}