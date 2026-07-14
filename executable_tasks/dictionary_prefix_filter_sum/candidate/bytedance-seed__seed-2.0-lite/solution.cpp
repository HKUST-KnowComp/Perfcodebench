#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Precompute validity of each dictionary entry once to avoid repeated work
  std::vector<bool> is_valid(dictionary.size(), false);
  for (size_t dict_idx = 0; dict_idx < dictionary.size(); ++dict_idx) {
    const std::string& s = dictionary[dict_idx];
    // Exact original prefix check to preserve full correctness
    if (s[0] == 'q' && s[1] <= 'm') {
      is_valid[dict_idx] = true;
    }
  }

  uint64_t final_sum = 0;
  const size_t num_codes = codes.size();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    // Tight cache-friendly inner loop with only primitive type accesses
    for (size_t i = 0; i < num_codes; ++i) {
      const size_t code = static_cast<size_t>(codes[i]);
      if (is_valid[code]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
    final_sum = sum;
  }
  return final_sum;
}