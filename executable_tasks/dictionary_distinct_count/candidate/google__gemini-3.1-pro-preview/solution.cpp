#include "interface.h"

#include <algorithm>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  
  if (codes.empty()) {
    return 0;
  }

  // Pre-allocate buffers outside the loop to minimize allocation overhead during iterations.
  // 'used' tracks which dictionary indices are present in the 'codes' vector.
  std::vector<uint8_t> used(dictionary.size());
  // 'used_strings' stores string_views of the strings actually referenced by 'codes'.
  std::vector<std::string_view> used_strings;
  used_strings.reserve(dictionary.size());

  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset buffers for the current iteration.
    std::fill(used.begin(), used.end(), 0);
    used_strings.clear();

    // Step 1: Identify which dictionary indices are used.
    // This is O(N) where N is the number of codes.
    for (uint16_t code : codes) {
      used[code] = 1;
    }

    // Step 2: Collect string_views for the used dictionary indices.
    // This is O(D) where D is the dictionary size.
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
      if (used[i]) {
        used_strings.emplace_back(dictionary[i]);
      }
    }

    // Step 3: Sort and count unique strings.
    // Sorting string_views is efficient as it avoids copying the underlying string data.
    // This is O(U log U) where U is the number of unique dictionary indices used.
    if (!used_strings.empty()) {
      std::sort(used_strings.begin(), used_strings.end());
      auto last = std::unique(used_strings.begin(), used_strings.end());
      count = static_cast<uint64_t>(std::distance(used_strings.begin(), last));
    } else {
      count = 0;
    }
  }

  return count;
}