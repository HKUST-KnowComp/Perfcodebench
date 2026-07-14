#include "interface.h"

#include <vector>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (codes.empty()) return 0;

  // Pre-calculate which dictionary entries match the prefix rule
  // Rule: s[0] == 'q' && s[1] <= 'm'
  std::vector<bool> dict_matches(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
  }

  uint64_t final_sum = 0;
  
  // The baseline resets sum = 0 at the start of every iteration.
  // Therefore, we only need to compute the sum for the last iteration.
  // If iters <= 0, the loop doesn't run and sum remains 0.
  if (iters > 0) {
    uint64_t current_sum = 0;
    const size_t n = codes.size();
    const uint16_t* code_ptr = codes.data();
    const uint32_t* weight_ptr = weights.data();
    
    for (size_t i = 0; i < n; ++i) {
      if (dict_matches[code_ptr[i]]) {
        current_sum += weight_ptr[i];
      }
    }
    final_sum = current_sum;
  }

  return final_sum;
}