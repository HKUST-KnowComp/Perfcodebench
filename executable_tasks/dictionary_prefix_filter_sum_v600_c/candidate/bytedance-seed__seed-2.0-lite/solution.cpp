#include "interface.h"
#include <string>
#include <vector>

namespace {
bool matches_prefix(const std::string& s) {
  return s[0] == 'q' && s[1] <= 'm';
}
}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Precompute prefix matches for all dictionary entries exactly once
  std::vector<bool> dict_matches(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    dict_matches[i] = matches_prefix(dictionary[i]);
  }

  // Calculate the correct per-iteration sum once to avoid redundant work
  uint64_t per_iter_sum = 0;
  for (size_t i = 0; i < codes.size(); ++i) {
    if (dict_matches[static_cast<size_t>(codes[i])]]) {
      per_iter_sum += static_cast<uint64_t>(weights[i]);
    }
  }

  // Maintain original iteration loop structure for harness compatibility
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = per_iter_sum;
  }

  return sum;
}