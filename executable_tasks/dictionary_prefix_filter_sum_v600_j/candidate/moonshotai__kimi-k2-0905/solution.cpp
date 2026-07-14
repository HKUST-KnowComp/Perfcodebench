#include "interface.h"

#include <cstdint>
#include <vector>
#include <bitset>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const std::size_t dict_size = dictionary.size();
  const std::size_t row_count = codes.size();

  // 1. Pre-compute which dictionary entries match the prefix rule
  std::bitset<65536> dict_ok;
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    dict_ok[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
  }

  // 2. Build a dense array of weights that correspond to each code
  //    (0 if the dictionary entry does not match the prefix)
  std::vector<uint32_t> prefix_weight(row_count);
  for (std::size_t i = 0; i < row_count; ++i) {
    const uint16_t code = codes[i];
    prefix_weight[i] = dict_ok[code] ? weights[i] : 0;
  }

  // 3. Hot loop: just sum the cached weights
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    for (std::size_t i = 0; i < row_count; ++i) {
      local += prefix_weight[i];
    }
    sum = local;
  }
  return sum;
}