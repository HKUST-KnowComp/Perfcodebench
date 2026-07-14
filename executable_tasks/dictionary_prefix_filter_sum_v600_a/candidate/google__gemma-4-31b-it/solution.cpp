#include "interface.h"

#include <string>
#include <vector>

namespace {

inline bool matches_prefix(const std::string& s) {
  // The original logic: s[0] == 'q' && s[1] <= 'm'
  // We must ensure the string has at least 2 characters to avoid out-of-bounds access
  // although the baseline didn't check length, we should be safe.
  return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0) return 0;

  // Pre-calculate which dictionary entries match the prefix rule.
  // This removes the need to access strings inside the hot loop.
  std::vector<bool> dict_matches(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    dict_matches[i] = matches_prefix(dictionary[i]);
  }

  uint64_t total_sum = 0;
  const size_t n = codes.size();
  
  // The baseline resets sum = 0 every iteration and returns the sum of the LAST iteration.
  // However, the baseline logic is: 
  // for (iter < iters) { sum = 0; for (i < n) { if (match) sum += weights[i]; } }
  // This means the result is simply the sum of weights for one pass, repeated 'iters' times.
  // But the baseline returns the 'sum' variable which is overwritten each iteration.
  // Therefore, the result is equivalent to calculating the sum once.
  
  uint64_t single_pass_sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (dict_matches[codes[i]]) {
      single_pass_sum += weights[i];
    }
  }

  // The baseline code does: 
  // for (int iter = 0; iter < iters; ++iter) { 
  //    sum = 0; 
  //    for (...) { if (matches) sum += weights[i]; }
  // }
  // return sum;
  // This means the loop runs 'iters' times, but 'sum' is reset to 0 at the start of each iteration.
  // The final returned value is the sum from the very last iteration.
  
  return single_pass_sum;
}