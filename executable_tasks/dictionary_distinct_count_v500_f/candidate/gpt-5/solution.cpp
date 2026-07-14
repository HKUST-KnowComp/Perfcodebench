#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <string_view>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int /*iters*/) {
  // Fast exits
  if (codes.empty() || dictionary.empty()) {
    return 0ull;
  }

  // Collect unique dictionary indices referenced by codes to minimize work.
  const std::size_t dict_size = dictionary.size();
  std::vector<uint8_t> seen_idx(dict_size, uint8_t{0});
  std::vector<uint32_t> unique_indices;
  unique_indices.reserve(std::min<std::size_t>(codes.size(), dict_size));

  for (uint16_t c : codes) {
    std::size_t idx = static_cast<std::size_t>(c);
    if (idx >= dict_size) {
      // Guard against out-of-bounds codes; skip invalid indices if any.
      continue;
    }
    if (!seen_idx[idx]) {
      seen_idx[idx] = 1;
      unique_indices.push_back(static_cast<uint32_t>(idx));
    }
  }

  if (unique_indices.empty()) {
    return 0ull;
  }

  // Use string_view to avoid copying strings; dictionary strings stay alive
  // for the duration of this function.
  std::unordered_set<std::string_view> distinct;
  distinct.reserve(unique_indices.size());

  for (uint32_t idx : unique_indices) {
    distinct.insert(std::string_view(dictionary[static_cast<std::size_t>(idx)]));
  }

  return static_cast<uint64_t>(distinct.size());
}
