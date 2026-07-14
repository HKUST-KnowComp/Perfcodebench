#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t dict_size = dictionary.size();
  if (dict_size == 0 || codes.empty()) return 0;

  // Count distinct dictionary indices referenced by codes.
  // Distinct decoded strings may be fewer if multiple dictionary entries hold
  // identical strings, so we first mark used indices, then count distinct
  // strings among only those used entries.
  std::vector<unsigned char> used(dict_size, 0);
  for (uint16_t code : codes) {
    const std::size_t idx = static_cast<std::size_t>(code);
    if (idx < dict_size) used[idx] = 1;
  }

  std::vector<std::size_t> active_indices;
  active_indices.reserve(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    if (used[i]) active_indices.push_back(i);
  }

  if (active_indices.empty()) return 0;
  if (active_indices.size() == 1) return 1;

  std::vector<unsigned char> unique_active(active_indices.size(), 1);
  uint64_t distinct = 0;

  for (std::size_t i = 0; i < active_indices.size(); ++i) {
    if (!unique_active[i]) continue;
    ++distinct;
    const std::string& si = dictionary[active_indices[i]];
    for (std::size_t j = i + 1; j < active_indices.size(); ++j) {
      if (unique_active[j] && dictionary[active_indices[j]] == si) {
        unique_active[j] = 0;
      }
    }
  }

  (void)iters;
  return distinct;
}
