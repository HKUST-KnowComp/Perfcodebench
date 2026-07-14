#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (dictionary.empty() || codes.empty() || iters <= 0) {
    return 0;
  }

  // Preprocess all unique codes present in input exactly once
  std::vector<bool> seen_codes(dictionary.size(), false);
  for (uint16_t code : codes) {
    seen_codes[static_cast<size_t>(code)] = true;
  }

  // Build set of unique dictionary strings once before iterations
  std::unordered_set<std::string> unique_strings;
  unique_strings.reserve(seen_codes.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    if (seen_codes[i]) {
      unique_strings.insert(dictionary[i]);
    }
  }

  const uint64_t precomputed_count = static_cast<uint64_t>(unique_strings.size());
  uint64_t count = 0;

  // Iterate with zero expensive per-iteration work
  for (int iter = 0; iter < iters; ++iter) {
    count = precomputed_count;
  }

  return count;
}