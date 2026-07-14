#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

struct StrPtrHash {
  std::size_t operator()(const std::string* s) const noexcept {
    return std::hash<std::string>{}(*s);
  }
};

struct StrPtrEq {
  bool operator()(const std::string* a, const std::string* b) const noexcept {
    return *a == *b;
  }
};

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int /*iters*/) {
  if (codes.empty()) return 0;

  // Deduplicate indices referenced by codes (codes are uint16_t => max 65536 distinct indices)
  std::vector<uint8_t> seen_idx(65536u, 0);
  std::vector<uint16_t> unique_indices;
  unique_indices.reserve(codes.size() < 65536u ? codes.size() : 65536u);

  for (uint16_t code : codes) {
    if (!seen_idx[code]) {
      seen_idx[code] = 1;
      unique_indices.push_back(code);
    }
  }

  // Count unique strings among the referenced dictionary entries using their content
  std::unordered_set<const std::string*, StrPtrHash, StrPtrEq> uniq;
  uniq.reserve(unique_indices.size());

  for (uint16_t idx : unique_indices) {
    // Assuming codes are valid indices into dictionary per task contract
    uniq.insert(&dictionary[static_cast<std::size_t>(idx)]);
  }

  return static_cast<uint64_t>(uniq.size());
}
