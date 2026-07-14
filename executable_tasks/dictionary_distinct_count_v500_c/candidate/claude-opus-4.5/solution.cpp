#include "interface.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Build a mapping from each unique string to a canonical index
  std::unordered_map<std::string, uint16_t> str_to_canonical;
  str_to_canonical.reserve(dictionary.size());
  std::vector<uint16_t> dict_canonical(dictionary.size());
  uint16_t next_canonical = 0;
  for (size_t i = 0; i < dictionary.size(); ++i) {
    auto it = str_to_canonical.find(dictionary[i]);
    if (it == str_to_canonical.end()) {
      str_to_canonical[dictionary[i]] = next_canonical;
      dict_canonical[i] = next_canonical;
      ++next_canonical;
    } else {
      dict_canonical[i] = it->second;
    }
  }

  // Precompute canonical indices for all codes
  std::vector<uint16_t> code_canonical(codes.size());
  for (size_t i = 0; i < codes.size(); ++i) {
    code_canonical[i] = dict_canonical[codes[i]];
  }

  // Bitset for tracking seen canonical indices
  // next_canonical <= dictionary.size() <= 65536 (uint16_t max + 1)
  const size_t bitset_size = (static_cast<size_t>(next_canonical) + 63) / 64;
  std::vector<uint64_t> seen(bitset_size);

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Clear bitset
    for (size_t i = 0; i < bitset_size; ++i) {
      seen[i] = 0;
    }
    uint64_t local_count = 0;
    for (size_t i = 0; i < code_canonical.size(); ++i) {
      uint16_t c = code_canonical[i];
      size_t word = c >> 6;
      uint64_t bit = static_cast<uint64_t>(1) << (c & 63);
      if (!(seen[word] & bit)) {
        seen[word] |= bit;
        ++local_count;
      }
    }
    count = local_count;
  }
  return count;
}
