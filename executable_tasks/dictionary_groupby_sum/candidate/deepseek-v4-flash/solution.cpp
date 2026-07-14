#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  // Build mapping from each dictionary entry to its unique string index
  std::unordered_map<std::string, int> str_to_idx;
  std::vector<int> dict_to_key(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    auto it = str_to_idx.find(dictionary[i]);
    if (it == str_to_idx.end()) {
      int idx = static_cast<int>(str_to_idx.size());
      str_to_idx.emplace(dictionary[i], idx);
      dict_to_key[i] = idx;
    } else {
      dict_to_key[i] = it->second;
    }
  }
  const int unique_count = static_cast<int>(str_to_idx.size());
  std::vector<uint64_t> sums(unique_count, 0);

  uint64_t hash = 0;
  const uint64_t offset = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(sums.begin(), sums.end(), 0);
    // Accumulate values per unique key using integer indexing (no string hashing)
    for (size_t i = 0; i < codes.size(); ++i) {
      sums[dict_to_key[static_cast<size_t>(codes[i])]] += static_cast<uint64_t>(values[i]);
    }
    // Checksum in dictionary order (duplicate strings each contribute separately)
    hash = offset;
    for (size_t d = 0; d < dictionary.size(); ++d) {
      hash = mix(hash, sums[dict_to_key[d]]);
    }
  }
  return hash;
}
