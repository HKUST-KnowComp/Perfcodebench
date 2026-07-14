#include "interface.h"

namespace {
inline bool dict_matches(const std::string& s) noexcept {
  return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}
}

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  // Precompute which dictionary entries match the prefix rule.
  const std::size_t dict_sz = dictionary.size();
  std::vector<uint8_t> match(dict_sz);
  for (std::size_t i = 0; i < dict_sz; ++i) {
    match[i] = static_cast<uint8_t>(dict_matches(dictionary[i]));
  }

  // Single pass over codes/weights using the precomputed mask.
  const std::size_t n = codes.size();
  const uint16_t* c = codes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(w[i]) * static_cast<uint64_t>(match[static_cast<std::size_t>(c[i])]);
  }

  return sum;
}
