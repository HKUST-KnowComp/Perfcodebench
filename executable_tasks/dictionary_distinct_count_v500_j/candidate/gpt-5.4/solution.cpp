#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;

  const std::size_t dict_size = dictionary.size();
  if (dict_size == 0 || codes.empty() || iters <= 0) {
    return 0;
  }

  // Fast path: codes are uint16_t, so when the dictionary fits in 65536 entries
  // each distinct decoded string corresponds exactly to a distinct code value.
  if (dict_size <= 65536u) {
    for (int iter = 0; iter < iters; ++iter) {
      bool seen[65536] = {};
      uint64_t distinct = 0;
      for (uint16_t c : codes) {
        if (!seen[c]) {
          seen[c] = true;
          ++distinct;
        }
      }
      count = distinct;
    }
    return count;
  }

  // Fallback for oversized dictionaries: distinct decoded strings are counted by
  // comparing dictionary entries referenced by codes.
  for (int iter = 0; iter < iters; ++iter) {
    std::vector<std::size_t> unique_indices;
    unique_indices.reserve(codes.size());

    for (uint16_t c : codes) {
      const std::size_t idx = static_cast<std::size_t>(c);
      bool found = false;
      for (std::size_t existing : unique_indices) {
        if (dictionary[existing] == dictionary[idx]) {
          found = true;
          break;
        }
      }
      if (!found) unique_indices.push_back(idx);
    }

    count = static_cast<uint64_t>(unique_indices.size());
  }

  return count;
}
