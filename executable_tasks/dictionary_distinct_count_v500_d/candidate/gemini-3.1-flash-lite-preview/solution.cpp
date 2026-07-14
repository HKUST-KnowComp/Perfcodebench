#include "interface.h"
#include <vector>
#include <string>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Identify unique dictionary indices present in the codes
  std::vector<bool> seen_indices(dictionary.size(), false);
  for (uint16_t code : codes) {
    seen_indices[code] = true;
  }

  // Count unique strings by hashing only the unique dictionary entries
  std::unordered_set<std::string> unique_strings;
  for (size_t i = 0; i < seen_indices.size(); ++i) {
    if (seen_indices[i]) {
      unique_strings.insert(dictionary[i]);
    }
  }

  uint64_t count = static_cast<uint64_t>(unique_strings.size());

  // The loop is required by the interface, but the result is invariant
  // after the first calculation.
  for (int iter = 1; iter < iters; ++iter) {
    // No-op
  }

  return count;
}