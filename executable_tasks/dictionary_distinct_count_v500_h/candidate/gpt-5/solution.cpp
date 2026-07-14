#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

// Hash and equality for pointers to std::string based on string contents
struct StringPtrHash {
  std::size_t operator()(const std::string* s) const noexcept {
    return std::hash<std::string>()(*s);
  }
};
struct StringPtrEq {
  bool operator()(const std::string* a, const std::string* b) const noexcept {
    return *a == *b;
  }
};

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;
  if (codes.empty() || dictionary.empty()) return 0;

  const std::size_t dict_size = dictionary.size();

  // Collect unique dictionary indices referenced by codes to avoid redundant hashing
  std::vector<uint8_t> used(dict_size, 0);
  std::vector<std::size_t> unique_indices;
  unique_indices.reserve(std::min<std::size_t>(codes.size(), dict_size));

  for (std::size_t i = 0; i < codes.size(); ++i) {
    std::size_t idx = static_cast<std::size_t>(codes[i]);
    // Assuming valid indices as in baseline (no bounds checks for performance)
    if (!used[idx]) {
      used[idx] = 1;
      unique_indices.push_back(idx);
    }
  }

  std::unordered_set<const std::string*, StringPtrHash, StringPtrEq> seen;
  seen.reserve(unique_indices.size());
  seen.max_load_factor(0.7f);

  for (std::size_t idx : unique_indices) {
    seen.insert(&dictionary[idx]);
  }

  return static_cast<uint64_t>(seen.size());
}
