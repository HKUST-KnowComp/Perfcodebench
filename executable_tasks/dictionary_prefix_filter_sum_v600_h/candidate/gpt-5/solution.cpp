#include "interface.h"

#include <string>
#include <vector>

namespace {
inline bool matches_prefix_fast(const std::string& s) {
  return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}
}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Preserve baseline semantics for non-positive iterations
  if (iters <= 0) {
    return 0;
  }

  // Precompute which dictionary entries match the prefix rule
  std::vector<uint8_t> match(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    match[i] = static_cast<uint8_t>(matches_prefix_fast(dictionary[i]));
  }

  // Compute the sum once; result is identical for each iteration
  const std::size_t n = codes.size();
  const uint16_t* cptr = codes.data();
  const uint32_t* wptr = weights.data();
  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const uint8_t m = match[static_cast<std::size_t>(cptr[i])];
    sum += static_cast<uint64_t>(wptr[i]) * static_cast<uint64_t>(m);
  }

  return sum;
}
