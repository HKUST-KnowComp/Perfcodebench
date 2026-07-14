#include "interface.h"
#include <string_view>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Use string_view to avoid copying the actual string content into the set.
  // The dictionary strings are guaranteed to live as long as the function call.
  std::unordered_set<std::string_view> seen;
  // Pre-reserve to minimize reallocations during the first iteration.
  seen.reserve(codes.size());

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    for (const uint16_t code : codes) {
      // Direct access to dictionary via code index.
      // We use string_view to point to the existing memory in the dictionary.
      seen.insert(dictionary[static_cast<std::size_t>(code)]);
    }
    count = static_cast<uint64_t>(seen.size());
  }

  return count;
}