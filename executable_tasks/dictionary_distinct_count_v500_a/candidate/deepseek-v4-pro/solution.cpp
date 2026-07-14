#include "interface.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;

  // Assign a unique integer ID to each distinct string in the dictionary.
  std::unordered_map<std::string, uint32_t> str_to_id;
  std::vector<uint32_t> dict_id(dictionary.size());
  uint32_t next_id = 0;
  for (size_t i = 0; i < dictionary.size(); ++i) {
    auto it = str_to_id.find(dictionary[i]);
    if (it == str_to_id.end()) {
      str_to_id[dictionary[i]] = next_id;
      dict_id[i] = next_id;
      ++next_id;
    } else {
      dict_id[i] = it->second;
    }
  }

  const uint32_t M = next_id;
  std::vector<bool> seen(M, false);
  uint64_t count = 0;

  for (size_t i = 0; i < codes.size(); ++i) {
    uint32_t id = dict_id[static_cast<size_t>(codes[i])];
    if (!seen[id]) {
      seen[id] = true;
      ++count;
    }
  }

  // The distinct count does not depend on the number of iterations.
  (void)iters;
  return count;
}