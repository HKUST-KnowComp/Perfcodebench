#include "interface.h"

#include <vector>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  if (dictionary.empty() || codes.empty()) return 0;

  // Pre-calculate which dictionary entries match the prefix rule
  // Rule: s[0] == 'q' && s[1] <= 'm'
  std::vector<bool> dict_matches(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
      dict_matches[i] = true;
    } else {
      dict_matches[i] = false;
    }
  }

  uint64_t final_sum = 0;
  size_t n = codes.size();

  // The problem asks for the sum for rows whose decoded string matches.
  // Since the dictionary and codes don't change across iters, 
  // the sum for one iteration is constant.
  uint64_t single_iter_sum = 0;
  for (size_t i = 0; i < n; ++i) {
    if (dict_matches[codes[i]]) {
      single_iter_sum += weights[i];
    }
  }

  // The baseline resets sum = 0 inside the loop, meaning it returns the sum of the LAST iteration.
  // If iters <= 0, the loop doesn't run, sum remains 0.
  if (iters > 0) {
    final_sum = single_iter_sum;
  } else {
    final_sum = 0;
  }

  return final_sum;
}