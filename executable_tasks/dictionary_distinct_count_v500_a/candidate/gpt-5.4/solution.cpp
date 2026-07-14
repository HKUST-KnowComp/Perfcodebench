#include "interface.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  const std::size_t dict_size = dictionary.size();

  // Map each dictionary entry to a canonical id representing its decoded string.
  // Duplicate strings in the dictionary share the same id.
  std::vector<uint32_t> canonical(dict_size);
  canonical.reserve(dict_size);

  std::unordered_map<std::string_view, uint32_t> ids;
  ids.reserve(dict_size * 2 + 1);

  uint32_t next_id = 0;
  for (std::size_t i = 0; i < dict_size; ++i) {
    std::string_view sv(dictionary[i]);
    auto it = ids.find(sv);
    if (it == ids.end()) {
      const uint32_t id = next_id++;
      ids.emplace(sv, id);
      canonical[i] = id;
    } else {
      canonical[i] = it->second;
    }
  }

  // Generation-stamped visited array avoids clearing per iteration.
  std::vector<uint32_t> seen(next_id, 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t distinct = 0;

    if (gen == 0) {
      for (std::size_t i = 0; i < seen.size(); ++i) seen[i] = 0;
      gen = 1;
    }

    for (std::size_t i = 0, n = codes.size(); i < n; ++i) {
      const std::size_t code = static_cast<std::size_t>(codes[i]);
      const uint32_t id = canonical[code];
      if (seen[id] != gen) {
        seen[id] = gen;
        ++distinct;
      }
    }

    count = distinct;
    ++gen;
  }

  return count;
}
