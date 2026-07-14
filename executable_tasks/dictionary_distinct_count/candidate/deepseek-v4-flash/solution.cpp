#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Build canonical index for each dictionary string: if duplicates exist,
  // all map to the first occurrence.
  std::unordered_map<std::string, uint16_t> first_index;
  std::vector<uint16_t> canonical(dictionary.size());
  for (uint16_t i = 0; i < dictionary.size(); ++i) {
    auto it = first_index.find(dictionary[i]);
    if (it == first_index.end()) {
      first_index[dictionary[i]] = i;
      canonical[i] = i;
    } else {
      canonical[i] = it->second;
    }
  }

  // Count distinct canonical indices using a timestamped visited array
  // to avoid clearing each iteration.
  std::vector<uint64_t> visited(dictionary.size(), 0);
  uint64_t version = 1;
  uint64_t count = 0;
  for (uint16_t code : codes) {
    uint16_t idx = canonical[code];
    if (visited[idx] != version) {
      visited[idx] = version;
      ++count;
    }
  }
  // The result does not depend on iters; compute once and return.
  return count;
}
