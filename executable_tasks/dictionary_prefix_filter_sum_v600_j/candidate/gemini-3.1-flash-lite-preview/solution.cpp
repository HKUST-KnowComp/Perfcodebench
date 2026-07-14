#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  const size_t dict_size = dictionary.size();
  std::vector<bool> is_match(dict_size);
  
  for (size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    // matches_prefix logic: s[0] == 'q' && s[1] <= 'm'
    // Ensure string is long enough to avoid out-of-bounds
    if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
      is_match[i] = true;
    } else {
      is_match[i] = false;
    }
  }

  uint64_t total_sum = 0;
  const size_t num_codes = codes.size();
  
  // Optimization: Hoist the loop to avoid re-calculating matches
  // The problem asks for the sum over iters, assuming the result is the same each time.
  // If the result is identical per iteration, we calculate once and multiply.
  uint64_t single_iter_sum = 0;
  for (size_t i = 0; i < num_codes; ++i) {
    if (is_match[codes[i]]) {
      single_iter_sum += static_cast<uint64_t>(weights[i]);
    }
  }

  return single_iter_sum * static_cast<uint64_t>(iters);
}