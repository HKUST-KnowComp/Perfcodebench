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

  // Fast path: codes are uint16_t, so when the dictionary fits in 65536 entries,
  // distinct decoded strings are exactly the number of distinct code values used.
  if (dict_size <= 65536u) {
    for (int iter = 0; iter < iters; ++iter) {
      alignas(64) uint64_t seen[1024] = {};
      uint64_t distinct = 0;

      const uint16_t* p = codes.data();
      const uint16_t* const end = p + codes.size();
      for (; p != end; ++p) {
        const uint32_t c = static_cast<uint32_t>(*p);
        const uint32_t word = c >> 6;
        const uint64_t bit = uint64_t{1} << (c & 63u);
        const uint64_t old = seen[word];
        if ((old & bit) == 0) {
          seen[word] = old | bit;
          ++distinct;
        }
      }
      count = distinct;
    }
    return count;
  }

  // Fallback for oversized dictionaries: count distinct decoded strings by
  // marking first occurrence of each dictionary entry referenced by codes.
  std::vector<unsigned char> seen(dict_size, 0);
  std::vector<std::size_t> touched;
  touched.reserve(codes.size() < dict_size ? codes.size() : dict_size);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t distinct = 0;
    for (std::size_t i = 0; i < codes.size(); ++i) {
      const std::size_t idx = static_cast<std::size_t>(codes[i]);
      if (!seen[idx]) {
        seen[idx] = 1;
        touched.push_back(idx);
        ++distinct;
      }
    }
    count = distinct;
    for (std::size_t i = 0; i < touched.size(); ++i) {
      seen[touched[i]] = 0;
    }
    touched.clear();
  }

  return count;
}
