#include "interface.h"

#include <string>
#include <vector>

namespace {

// Inline the prefix check to allow the compiler to optimize the character comparisons.
// The rule is: s[0] == 'q' and s[1] <= 'm'.
// We must also ensure the string has at least 2 characters to avoid out-of-bounds access.
inline bool matches_prefix(const std::string& s) {
    return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  
  uint64_t total_sum = 0;
  const size_t n = codes.size();
  
  // Pre-calculate which dictionary indices satisfy the prefix condition.
  // This moves the string inspection logic outside the 'iters' loop.
  // We use a bitset-like approach or a boolean vector to store results.
  std::vector<bool> satisfies_prefix(dictionary.size(), false);
  for (size_t i = 0; i < dictionary.size(); ++i) {
    if (matches_prefix(dictionary[i])) {
      satisfies_prefix[i] = true;
    }
  }

  // The core loop now only performs integer lookups and additions.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // codes[i] is the index into the dictionary.
      // We check if that dictionary entry satisfies the prefix rule.
      if (satisfies_prefix[codes[i]]) {
        current_iter_sum += static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}