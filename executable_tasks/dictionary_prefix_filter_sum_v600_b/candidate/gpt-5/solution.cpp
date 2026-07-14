#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {
inline uint8_t dict_matches(const std::string& s) {
  if (s.size() < 2) return 0;
  const char* p = s.data();
  return static_cast<uint8_t>(p[0] == 'q' && p[1] <= 'm');
}
}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)iters; // Sum does not depend on iteration count; compute once.

  const std::size_t dict_sz = dictionary.size();
  std::vector<uint8_t> match(dict_sz);
  for (std::size_t i = 0; i < dict_sz; ++i) {
    match[i] = dict_matches(dictionary[i]);
  }

  uint64_t sum = 0;
  const std::size_t n = codes.size();
  if (n == 0) return sum;

  const uint16_t* codep = &codes[0];
  const uint32_t* wp = &weights[0];

  std::size_t i = 0;
  // Unrolled loop for better throughput
  for (; i + 4 <= n; i += 4) {
    uint16_t c0 = codep[i];
    uint16_t c1 = codep[i + 1];
    uint16_t c2 = codep[i + 2];
    uint16_t c3 = codep[i + 3];
    if (match[c0]) sum += static_cast<uint64_t>(wp[i]);
    if (match[c1]) sum += static_cast<uint64_t>(wp[i + 1]);
    if (match[c2]) sum += static_cast<uint64_t>(wp[i + 2]);
    if (match[c3]) sum += static_cast<uint64_t>(wp[i + 3]);
  }
  for (; i < n; ++i) {
    if (match[codep[i]]) sum += static_cast<uint64_t>(wp[i]);
  }

  return sum;
}
