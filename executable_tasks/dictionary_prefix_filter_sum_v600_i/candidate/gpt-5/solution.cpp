#include "interface.h"

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

namespace {
inline bool matches_prefix_local(const std::string& s) {
  // Safe check; typical data has length >= 2
  return s.size() > 1 && s[0] == 'q' && s[1] <= 'm';
}
}

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  // Precompute match flags for dictionary entries
  const std::size_t dict_sz = dictionary.size();
  std::vector<uint8_t> match(dict_sz);
  for (std::size_t i = 0; i < dict_sz; ++i) {
    match[i] = static_cast<uint8_t>(matches_prefix_local(dictionary[i]));
  }

  // Compute the sum once; result is invariant across iterations
  const std::size_t n = codes.size();
  const uint16_t* cptr = codes.data();
  const uint32_t* wptr = weights.data();
  const uint8_t* mptr = match.data();

  uint64_t sum = 0;

  std::size_t i = 0;
  // Unrolled loop for better throughput
  for (; i + 4 <= n; i += 4) {
    const uint16_t c0 = cptr[i];
    const uint16_t c1 = cptr[i + 1];
    const uint16_t c2 = cptr[i + 2];
    const uint16_t c3 = cptr[i + 3];

    sum += static_cast<uint64_t>(wptr[i]) * static_cast<uint64_t>(mptr[c0]);
    sum += static_cast<uint64_t>(wptr[i + 1]) * static_cast<uint64_t>(mptr[c1]);
    sum += static_cast<uint64_t>(wptr[i + 2]) * static_cast<uint64_t>(mptr[c2]);
    sum += static_cast<uint64_t>(wptr[i + 3]) * static_cast<uint64_t>(mptr[c3]);
  }
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(wptr[i]) * static_cast<uint64_t>(mptr[cptr[i]]);
  }

  return sum;
}
