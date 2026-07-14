#include "interface.h"

#include <string>
#include <vector>

namespace {
inline bool dict_entry_matches(const std::string& s) {
  if (s.size() < 2) return false;
  if (s[0] != 'q') return false;
  unsigned char c1 = static_cast<unsigned char>(s[1]);
  return c1 <= static_cast<unsigned char>('m');
}
}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  // Precompute which dictionary entries match the prefix rule
  std::vector<uint8_t> match_flag(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    match_flag[i] = dict_entry_matches(dictionary[i]) ? 1u : 0u;
  }

  // Single pass accumulation using precomputed flags (branchless)
  uint64_t sum = 0;
  const std::size_t n = codes.size();
  const uint16_t* cptr = codes.data();
  const uint32_t* wptr = weights.data();
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(wptr[i]) * match_flag[static_cast<std::size_t>(cptr[i])];
  }
  return sum;
}
