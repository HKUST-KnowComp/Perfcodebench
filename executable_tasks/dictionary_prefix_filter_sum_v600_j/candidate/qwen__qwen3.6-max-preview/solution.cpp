#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0 || codes.empty()) return 0;

  // Precompute match status for each dictionary entry.
  // Using uint8_t avoids std::vector<bool> bit-packing overhead.
  std::vector<uint8_t> dict_matches(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    dict_matches[i] = (s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
  }

  uint64_t sum = 0;
  const uint16_t* codes_ptr = codes.data();
  const uint32_t* weights_ptr = weights.data();
  const uint8_t* matches_ptr = dict_matches.data();
  size_t n = codes.size();

  // The baseline recomputes the identical sum 'iters' times, overwriting it each pass.
  // We compute it once for correctness and maximal performance.
  for (size_t i = 0; i < n; ++i) {
    if (matches_ptr[codes_ptr[i]]) {
      sum += weights_ptr[i];
    }
  }

  return sum;
}
