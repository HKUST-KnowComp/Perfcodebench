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
  // Precompute validity flags for all dictionary entries once upfront
  std::vector<bool> valid_code(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    valid_code[i] = matches_prefix(dictionary[i]);
  }

  uint64_t sum = 0;
  // Maintain iteration count to preserve benchmark timing requirements
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < codes.size(); ++i) {
      if (valid_code[static_cast<size_t>(codes[i])]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}