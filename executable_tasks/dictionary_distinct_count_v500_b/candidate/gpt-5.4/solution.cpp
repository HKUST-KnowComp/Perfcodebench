#include "interface.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  const std::size_t dict_size = dictionary.size();
  if (iters <= 0 || codes.empty() || dict_size == 0) {
    return 0;
  }

  // Map each dictionary entry to a canonical distinct-string id so that
  // duplicate strings in the dictionary are counted once.
  std::unordered_map<std::string, uint32_t> id_of;
  id_of.reserve(dict_size);

  std::vector<uint32_t> dict_id(dict_size);
  uint32_t distinct_dict_strings = 0;
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    auto it = id_of.find(s);
    if (it == id_of.end()) {
      const uint32_t id = distinct_dict_strings++;
      id_of.emplace(s, id);
      dict_id[i] = id;
    } else {
      dict_id[i] = it->second;
    }
  }

  std::vector<uint32_t> seen(distinct_dict_strings, 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (gen == 0) {
      // Extremely unlikely wraparound protection.
      for (std::size_t i = 0; i < seen.size(); ++i) seen[i] = 0;
      gen = 1;
    }

    uint64_t local_count = 0;
    for (std::size_t i = 0, n = codes.size(); i < n; ++i) {
      const std::size_t code = static_cast<std::size_t>(codes[i]);
      const uint32_t id = dict_id[code];
      if (seen[id] != gen) {
        seen[id] = gen;
        ++local_count;
      }
    }
    count = local_count;
    ++gen;
  }

  return count;
}
