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
  // We only store the pointer and length.
  std::unordered_set<std::string_view> seen;
  // Pre-reserve to reduce rehashes during the loop.
  seen.reserve(codes.size());

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    for (uint16_t code : codes) {
      // Directly access the dictionary entry via the code.
      // Using string_view avoids the allocation/copying of the baseline's 'decoded' vector.
      seen.insert(dictionary[static_cast<std::size_t>(code)]);
    }
    count = static_cast<uint64_t>(seen.size());
  }

  return count;
}