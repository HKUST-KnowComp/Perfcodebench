#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;
  if (codes.empty()) return 0;

  const std::size_t dict_size = dictionary.size();

  // Fast path: codes are 16-bit, so when the dictionary fits in that range we can
  // count distinct decoded strings by counting distinct code values directly.
  // Dictionary entries are addressed by code, so different valid codes correspond
  // to different dictionary strings.
  if (dict_size <= 65536u) {
    std::vector<uint8_t> seen(65536u, 0);
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
      count = 0;
      for (uint16_t code : codes) {
        uint8_t& slot = seen[code];
        if (!slot) {
          slot = 1;
          ++count;
        }
      }
      for (uint16_t code : codes) {
        seen[code] = 0;
      }
    }
    return count;
  }

  // Fallback for oversized dictionaries: mark by index in a compact byte array.
  std::vector<uint8_t> seen(dict_size, 0);
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (uint16_t code : codes) {
      const std::size_t idx = static_cast<std::size_t>(code);
      uint8_t& slot = seen[idx];
      if (!slot) {
        slot = 1;
        ++count;
      }
    }
    for (uint16_t code : codes) {
      seen[static_cast<std::size_t>(code)] = 0;
    }
  }
  return count;
}
