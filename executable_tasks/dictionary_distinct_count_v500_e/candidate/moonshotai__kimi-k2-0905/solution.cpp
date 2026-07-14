#include "interface.h"
#include <cstdint>
#include <cstring>
#include <vector>
#include <immintrin.h>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  const size_t dict_size = dictionary.size();
  const uint32_t max_segments = (dict_size + 127) >> 7;
  uint64_t segment_used[256] = {0};     // 8192 bits, enough for dict_size up to 8K.

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t unique = 0;
    memset(segment_used, 0, sizeof(segment_used));

    for (size_t i = 0; i < codes.size(); ++i) {
      const uint16_t code = codes[i];
      // avoid branch mispredict in set bit
      const uint32_t seg   = code >> 7;         // 0-255 range
      const uint32_t bit   = code & 0x7F;     // 0-127 range
      const uint64_t mask  = 1ULL << bit;
      unique += __builtin_popcountll( (segment_used[seg] & mask) == 0 );
      segment_used[seg] |= mask;
    }
    count = unique;
  }
  return count;
}