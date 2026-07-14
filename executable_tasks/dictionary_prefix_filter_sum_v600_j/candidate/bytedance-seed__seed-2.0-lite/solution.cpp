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
  // Precompute validity lookup once, outside the hot iteration loop
  std::vector<bool> is_valid(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    is_valid[i] = matches_prefix(dictionary[i]);
  }

  uint64_t final_sum = 0;
  const std::size_t n = codes.size();
  // Hot loop: run sum calculation iters times with no repeated string work
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (is_valid[static_cast<std::size_t>(codes[i])]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
    final_sum = sum;
  }
  return final_sum;
}