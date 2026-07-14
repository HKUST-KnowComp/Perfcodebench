#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <string_view>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) {
    return 0;
  }

  uint64_t count = 0;
  // Use string_view to avoid copying the actual string data from the dictionary.
  // This makes the set store only a pointer and a length.
  std::unordered_set<std::string_view> seen;
  // Pre-reserve to avoid rehashes during the loop.
  seen.reserve(codes.size());

  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    for (const uint16_t code : codes) {
      // Access the dictionary directly via the code.
      // string_view points to the existing memory in the dictionary vector.
      seen.insert(dictionary[static_cast<std::size_t>(code)]);
    }
    count = static_cast<uint64_t>(seen.size());
  }

  return count;
}