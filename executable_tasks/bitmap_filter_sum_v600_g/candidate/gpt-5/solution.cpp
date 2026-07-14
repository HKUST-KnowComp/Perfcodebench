#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t /*key_space*/,
    int /*iters*/) {
  const std::size_t n = probe_keys.size();
  if (n == 0 || enabled_keys.empty()) {
    return 0ULL;
  }

  // Determine the maximum key seen to size the bitmap minimally.
  uint32_t max_key = 0;
  for (uint32_t k : enabled_keys) {
    if (k > max_key) max_key = k;
  }
  for (uint32_t k : probe_keys) {
    if (k > max_key) max_key = k;
  }

  // Number of 64-bit words needed to store bits up to max_key.
  const std::size_t words = (static_cast<std::size_t>(max_key) >> 6) + 1;
  std::vector<uint64_t> bitmap(words, 0ULL);

  // Build bitmap of enabled keys.
  for (uint32_t k : enabled_keys) {
    const std::size_t wi = static_cast<std::size_t>(k) >> 6; // divide by 64
    const uint32_t bi = k & 63U;                             // mod 64
    bitmap[wi] |= (1ULL << bi);
  }

  // Sum values where probe key is enabled using branchless add.
  const uint32_t* pk = probe_keys.data();
  const uint32_t* vv = values.data();
  uint64_t sum = 0ULL;

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t k = pk[i];
    const std::size_t wi = static_cast<std::size_t>(k) >> 6;
    const uint32_t bi = k & 63U;
    const uint64_t word = bitmap[wi];
    const uint64_t flag = (word >> bi) & 1ULL; // 1 if enabled, else 0
    const uint64_t add = static_cast<uint64_t>(vv[i]);
    sum += add & (0ULL - flag); // add when flag==1, else add 0
  }

  return sum;
}
