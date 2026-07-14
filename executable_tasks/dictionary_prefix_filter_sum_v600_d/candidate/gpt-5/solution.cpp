#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {
inline uint8_t matches_prefix_fast(const std::string& s) {
  if (s.size() < 2) return 0;
  if (s[0] != 'q') return 0;
  return static_cast<unsigned char>(s[1]) <= static_cast<unsigned char>('m');
}
}

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  // Precompute which dictionary entries match the prefix rule
  const std::size_t dict_size = dictionary.size();
  std::vector<uint8_t> match(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    match[i] = matches_prefix_fast(dictionary[i]);
  }

  const uint16_t* cptr = codes.data();
  const uint32_t* wptr = weights.data();
  const std::size_t n = codes.size();

  uint64_t sum = 0;

  // Unrolled accumulation with branchless add using 0/1 mask
  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    const uint16_t c0 = cptr[i];
    const uint16_t c1 = cptr[i + 1];
    const uint16_t c2 = cptr[i + 2];
    const uint16_t c3 = cptr[i + 3];

    sum += static_cast<uint64_t>(wptr[i])     * match[c0];
    sum += static_cast<uint64_t>(wptr[i + 1]) * match[c1];
    sum += static_cast<uint64_t>(wptr[i + 2]) * match[c2];
    sum += static_cast<uint64_t>(wptr[i + 3]) * match[c3];
  }
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(wptr[i]) * match[cptr[i]];
  }

  return sum;
}
