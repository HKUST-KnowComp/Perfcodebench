#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  const std::size_t dict_size = dictionary.size();
  if (iters <= 0) return 0;
  if (codes.empty() || dict_size == 0) return 0;

  std::vector<uint32_t> seen(dict_size, 0);
  uint32_t gen = 1;
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t distinct = 0;

    for (uint16_t code : codes) {
      const std::size_t idx = static_cast<std::size_t>(code);
      if (seen[idx] != gen) {
        seen[idx] = gen;
        ++distinct;
      }
    }

    count = distinct;

    ++gen;
    if (gen == 0) {
      for (std::size_t i = 0; i < dict_size; ++i) seen[i] = 0;
      gen = 1;
    }
  }

  return count;
}
