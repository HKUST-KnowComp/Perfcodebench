#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  const std::size_t dict_size = dictionary.size();
  const std::size_t n = codes.size();

  // Aggregate weights per dictionary code once.
  std::vector<uint64_t> weight_by_code(dict_size, 0);
  const uint16_t* cptr = codes.data();
  const uint32_t* wptr = weights.data();
  for (std::size_t i = 0; i < n; ++i) {
    weight_by_code[static_cast<std::size_t>(cptr[i])] += static_cast<uint64_t>(wptr[i]);
  }

  // Sum weights for entries whose strings match the prefix predicate.
  uint64_t sum = 0;
  for (std::size_t d = 0; d < dict_size; ++d) {
    uint64_t w = weight_by_code[d];
    if (!w) continue;  // Skip unused dictionary entries without touching strings.
    const std::string& s = dictionary[d];
    if (s[0] == 'q' && s[1] <= 'm') {
      sum += w;
    }
  }

  return sum;
}
