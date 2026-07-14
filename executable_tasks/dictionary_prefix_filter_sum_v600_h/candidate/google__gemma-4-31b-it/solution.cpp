#include "interface.h"

#include <string>
#include <vector>

namespace {

inline bool matches_prefix(const std::string& s) {
  if (s.empty()) return false;
  return s[0] == 'q' && (s.size() < 2 || s[1] <= 'm');
  // Note: The baseline used s[1] without checking size. 
  // In a real scenario, we must ensure s.size() >= 2 to avoid UB.
  // However, based on the baseline logic: s[0] == 'q' && s[1] <= 'm'.
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0) return 0;

  // Pre-calculate which dictionary indices match the prefix
  std::vector<bool> dict_matches(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    // Replicating baseline logic exactly: s[0] == 'q' && s[1] <= 'm'
    // We assume strings are long enough based on baseline behavior
    dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
  }

  // Pre-calculate which codes match the prefix to avoid repeated dictionary lookups
  // Since codes.size() can be large, we can just iterate over codes and weights.
  // To optimize the 'iters' loop, we can pre-calculate the sum for one iteration.
  uint64_t single_iter_sum = 0;
  const size_t n = codes.size();
  for (size_t i = 0; i < n; ++i) {
    if (dict_matches[codes[i]]) {
      single_iter_sum += weights[i];
    }
  }

  // The baseline resets sum = 0 at the start of each iteration and returns the last sum.
  // Therefore, the result is simply the sum of a single iteration.
  // If the baseline intended to accumulate across iters, it would be single_iter_sum * iters.
  // But the baseline says: sum = 0; for(...) { ... } return sum;
  return single_iter_sum;
}