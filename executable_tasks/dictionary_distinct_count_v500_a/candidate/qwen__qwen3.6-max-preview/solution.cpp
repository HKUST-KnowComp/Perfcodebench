#include "interface.h"
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <string>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Map each dictionary index to a canonical index (first occurrence of that string)
  // This ensures correctness even if the dictionary contains duplicate strings.
  std::vector<uint16_t> canonical(dictionary.size());
  std::unordered_map<std::string, uint16_t> str_to_id;
  str_to_id.reserve(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) {
    auto res = str_to_id.emplace(dictionary[i], static_cast<uint16_t>(i));
    canonical[i] = res.first->second;
  }

  // Fast lookup table for tracking seen canonical codes.
  // uint8_t avoids the bitwise overhead of std::vector<bool>.
  std::vector<uint8_t> seen(dictionary.size(), 0);
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (uint16_t c : codes) {
      uint16_t cid = canonical[c];
      if (!seen[cid]) {
        seen[cid] = 1;
        ++count;
      }
    }
    // Reset only the entries we touched. Iterating codes is sequential and cache-friendly.
    for (uint16_t c : codes) {
      seen[canonical[c]] = 0;
    }
  }
  return count;
}
