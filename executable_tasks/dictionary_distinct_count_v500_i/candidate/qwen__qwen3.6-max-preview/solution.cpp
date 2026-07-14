#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)iters;

  // Fast lookup for seen codes (uint16_t range: 0-65535)
  uint8_t seen_code[65536];
  std::memset(seen_code, 0, sizeof(seen_code));

  std::vector<const std::string*> unique_ptrs;
  unique_ptrs.reserve(codes.size() < 65536 ? codes.size() : 65536);

  // Deduplicate codes and collect pointers to dictionary strings
  for (uint16_t c : codes) {
    if (!seen_code[c]) {
      seen_code[c] = 1;
      unique_ptrs.push_back(&dictionary[c]);
    }
  }

  // Sort pointers by string content to group identical strings
  std::sort(unique_ptrs.begin(), unique_ptrs.end(),
            [](const std::string* a, const std::string* b) { return *a < *b; });

  // Count distinct strings in a single linear pass
  uint64_t count = 0;
  if (!unique_ptrs.empty()) {
    count = 1;
    for (size_t i = 1; i < unique_ptrs.size(); ++i) {
      if (*unique_ptrs[i] != *unique_ptrs[i - 1]) {
        ++count;
      }
    }
  }

  return count;
}
