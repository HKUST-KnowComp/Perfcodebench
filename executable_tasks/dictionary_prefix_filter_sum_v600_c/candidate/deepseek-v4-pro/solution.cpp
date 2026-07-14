#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const size_t dict_size = dictionary.size();
  std::vector<uint8_t> match(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    match[i] = (s[0] == 'q' && s[1] <= 'm');
  }

  const size_t n = codes.size();
  const uint16_t* code_data = codes.data();
  const uint32_t* weight_data = weights.data();

  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
      if (match[code_data[i]]) {
        sum += weight_data[i];
      }
    }
    final_sum = sum;
  }
  return final_sum;
}
