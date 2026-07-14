#include "interface.h"

#include <string>
#include <vector>

namespace {

// The prefix rule: s[0] == 'q' and s[1] <= 'm'
// We check length to prevent out-of-bounds access if strings are short.
inline bool matches_prefix(const std::string& s) {
    return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (codes.empty()) return 0;

  // Pre-calculate which dictionary entries satisfy the prefix condition.
  // This avoids repeated string access and logic evaluation inside the loop.
  std::vector<bool> dict_matches(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    dict_matches[i] = matches_prefix(dictionary[i]);
  }

  // Pre-calculate which indices in the 'codes' array satisfy the condition.
  // This transforms the problem into a simple sum of weights for specific indices.
  // We store the indices that match to avoid branching in the hot loop.
  std::vector<std::size_t> matching_indices;
  matching_indices.reserve(codes.size());
  for (std::size_t i = 0; i < codes.size(); ++i) {
    if (dict_matches[static_cast<std::size_t>(codes[i])]) {
      matching_indices.push_back(i);
    }
  }

  uint64_t total_sum = 0;
  // The hot loop now only performs additions using pre-filtered indices.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (std::size_t idx : matching_indices) {
      current_iter_sum += static_cast<uint64_t>(weights[idx]);
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}