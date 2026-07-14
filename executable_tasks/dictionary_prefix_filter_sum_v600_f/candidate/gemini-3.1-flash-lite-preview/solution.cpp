#include "interface.h"
#include <vector>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const size_t dict_size = dictionary.size();
  std::vector<bool> is_match(dict_size);

  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    is_match[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
  }

  uint64_t total_sum = 0;
  const size_t num_codes = codes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < num_codes; ++i) {
      if (is_match[codes[i]]) {
        current_iter_sum += weights[i];
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}