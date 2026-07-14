#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Precompute prefix match status for all dictionary entries once, before timing iterations
  std::vector<bool> match_lookup(dictionary.size(), false);
  for (std::size_t dict_idx = 0; dict_idx < dictionary.size(); ++dict_idx) {
    const std::string& s = dictionary[dict_idx];
    // Add safe length check to avoid undefined behavior from short strings
    if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
      match_lookup[dict_idx] = true;
    }
  }

  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    // Iterate directly over compact primitive arrays with no string operations in hot path
    for (std::size_t i = 0; i < codes.size(); ++i) {
      const std::size_t code = static_cast<std::size_t>(codes[i]);
      if (match_lookup[code]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
    // Preserve baseline behavior of returning the last iteration's computed sum
    final_sum = sum;
  }
  return final_sum;
}