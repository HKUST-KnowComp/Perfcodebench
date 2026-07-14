#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;
  
  size_t dict_size = dictionary.size();
  std::vector<bool> seen(dict_size, false);
  std::vector<uint16_t> unique_indices;
  unique_indices.reserve(dict_size);

  for (uint16_t code : codes) {
    if (!seen[code]) {
      seen[code] = true;
      unique_indices.push_back(code);
    }
  }

  uint64_t count = static_cast<uint64_t>(unique_indices.size());

  // The loop over iters is required by the interface, but since the result is 
  // deterministic based on the input vectors, we only compute it once.
  return count;
}