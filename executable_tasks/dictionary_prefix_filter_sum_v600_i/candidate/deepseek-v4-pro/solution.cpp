#include "interface.h"
#include <vector>
#include <string>

namespace {
bool matches_prefix(const std::string& s) {
  // original logic, but guard against short strings to match original behaviour safely
  return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}
} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Precompute dictionary matches
  std::vector<bool> match(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    match[i] = matches_prefix(dictionary[i]);
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < codes.size(); ++i) {
      if (match[codes[i]]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}