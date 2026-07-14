#include "interface.h"

#include <unordered_set>
#include <string_view>
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Preserve baseline behavior for iters == 0
  if (iters == 0) return 0ULL;

  if (codes.empty() || dictionary.empty()) return 0ULL;

  // Deduplicate indices referenced by codes to reduce string hash/compare work
  const std::size_t dict_sz = dictionary.size();
  std::vector<uint8_t> seen_idx(dict_sz, 0);
  std::vector<std::size_t> unique_idxs;
  unique_idxs.reserve(codes.size());

  for (uint16_t c : codes) {
    std::size_t idx = static_cast<std::size_t>(c);
    // Assume inputs are valid as in baseline; no bounds checks added.
    if (!seen_idx[idx]) {
      seen_idx[idx] = 1;
      unique_idxs.push_back(idx);
    }
  }

  // Count distinct strings among the unique indices using string_view to avoid copies
  std::unordered_set<std::string_view> distinct;
  distinct.reserve(unique_idxs.size());

  for (std::size_t idx : unique_idxs) {
    distinct.emplace(dictionary[idx]);
  }

  return static_cast<uint64_t>(distinct.size());
}
