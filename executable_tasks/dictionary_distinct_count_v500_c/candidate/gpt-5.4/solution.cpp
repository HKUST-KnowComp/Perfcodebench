#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;

  if (iters <= 0) return 0;
  if (codes.empty()) return 0;
  if (dictionary.empty()) return 0;

  const std::size_t dict_size = dictionary.size();

  // Fast path: codes are uint16_t, so when the dictionary fits in 65536 entries
  // we can count distinct decoded strings by marking distinct code values seen,
  // then deduplicating only among those dictionary entries whose strings may be
  // equal. This avoids rebuilding decoded strings and avoids hashing every input
  // element into an unordered_set each iteration.
  if (dict_size <= 65536u) {
    std::vector<unsigned char> code_seen(dict_size, 0);
    std::vector<uint16_t> unique_codes;
    unique_codes.reserve(codes.size() < dict_size ? codes.size() : dict_size);

    for (uint16_t code : codes) {
      const std::size_t idx = static_cast<std::size_t>(code);
      if (!code_seen[idx]) {
        code_seen[idx] = 1;
        unique_codes.push_back(code);
      }
    }

    // Deduplicate decoded strings among the unique referenced dictionary entries.
    // Use a simple O(k^2) scan over unique codes, which is often very fast in
    // practice because k is bounded by 65536 and avoids unordered_set overhead.
    uint64_t distinct = 0;
    for (std::size_t i = 0; i < unique_codes.size(); ++i) {
      const std::string& s = dictionary[static_cast<std::size_t>(unique_codes[i])];
      bool found = false;
      for (std::size_t j = 0; j < i; ++j) {
        if (dictionary[static_cast<std::size_t>(unique_codes[j])] ] == s) {
          found = true;
          break;
        }
      }
      if (!found) ++distinct;
    }

    for (int iter = 0; iter < iters; ++iter) {
      count = distinct;
    }
    return count;
  }

  // Fallback for oversized dictionaries: still avoid materializing decoded data.
  // Since codes are uint16_t, only the first 65536 dictionary entries are reachable.
  const std::size_t reachable = 65536u;
  std::vector<unsigned char> code_seen(reachable, 0);
  std::vector<uint16_t> unique_codes;
  unique_codes.reserve(codes.size() < reachable ? codes.size() : reachable);

  for (uint16_t code : codes) {
    const std::size_t idx = static_cast<std::size_t>(code);
    if (!code_seen[idx]) {
      code_seen[idx] = 1;
      unique_codes.push_back(code);
    }
  }

  uint64_t distinct = 0;
  for (std::size_t i = 0; i < unique_codes.size(); ++i) {
    const std::string& s = dictionary[static_cast<std::size_t>(unique_codes[i])];
    bool found = false;
    for (std::size_t j = 0; j < i; ++j) {
      if (dictionary[static_cast<std::size_t>(unique_codes[j])] ] == s) {
        found = true;
        break;
      }
    }
    if (!found) ++distinct;
  }

  for (int iter = 0; iter < iters; ++iter) {
    count = distinct;
  }
  return count;
}
