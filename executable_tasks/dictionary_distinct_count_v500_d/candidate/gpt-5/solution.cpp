#include "interface.h"

#include <unordered_set>
#include <vector>

struct PtrStringHash {
  std::size_t operator()(const std::string* p) const noexcept {
    return std::hash<std::string>()(*p);
  }
};
struct PtrStringEqual {
  bool operator()(const std::string* a, const std::string* b) const noexcept {
    return *a == *b;
  }
};

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;
  const std::size_t dict_size = dictionary.size();
  const std::size_t codes_size = codes.size();
  if (dict_size == 0 || codes_size == 0) return 0;

  // Deduplicate dictionary indices referenced by codes
  std::vector<uint8_t> used(dict_size, 0);
  std::vector<std::size_t> used_indices;
  std::size_t reserve_n = codes_size;
  if (reserve_n > dict_size) reserve_n = dict_size;
  used_indices.reserve(reserve_n);
  for (std::size_t i = 0; i < codes_size; ++i) {
    std::size_t idx = static_cast<std::size_t>(codes[i]);
    // Assuming codes are valid indices as per baseline behavior
    if (!used[idx]) {
      used[idx] = 1;
      used_indices.push_back(idx);
    }
  }

  // Count distinct strings among used indices without copying
  std::unordered_set<const std::string*, PtrStringHash, PtrStringEqual> distinct;
  distinct.reserve(used_indices.size());
  for (std::size_t idx : used_indices) {
    distinct.insert(&dictionary[idx]);
  }

  return static_cast<uint64_t>(distinct.size());
}
