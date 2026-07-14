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
  // Precompute validity mask for all dictionary entries once, not per iteration
  std::vector<bool> dict_valid(dictionary.size());
  for (size_t d = 0; d < dictionary.size(); ++d) {
    dict_valid[d] = matches_prefix(dictionary[d]);
  }

  uint64_t final_sum = 0;
  const size_t num_codes = codes.size();
  // Run calculation for required number of iterations with minimal overhead
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (size_t i = 0; i < num_codes; ++i) {
      if (dict_valid[static_cast<size_t>(codes[i])]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
    final_sum = sum;
  }
  return final_sum;
}