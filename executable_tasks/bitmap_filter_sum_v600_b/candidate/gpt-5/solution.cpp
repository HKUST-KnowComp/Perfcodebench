#include "interface.h"

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int /*iters*/) {
  // Early exits for trivial cases
  if (key_space == 0 || probe_keys.empty() || enabled_keys.empty()) {
    return 0ULL;
  }

  // Build a compact bitmap of enabled keys
  const uint32_t ks = key_space;
  const std::size_t bit_words = (static_cast<std::size_t>(ks) + 63u) >> 6; // words of 64 bits
  std::vector<uint64_t> bitmap(bit_words, 0ULL);

  // Set bits for enabled keys
  for (std::size_t i = 0; i < enabled_keys.size(); ++i) {
    uint32_t k = enabled_keys[i];
    // Keys are assumed to be within [0, key_space); per problem constraints
    const std::size_t w = static_cast<std::size_t>(k) >> 6;
    const uint32_t b = k & 63u;
    bitmap[w] |= (1ULL << b);
  }

  // Sum values for probe keys that are enabled
  const uint64_t* bm = bitmap.data();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* vals = values.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0ULL;

  // Unrolled loop for better throughput
  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    uint32_t k0 = pk[i + 0];
    uint32_t k1 = pk[i + 1];
    uint32_t k2 = pk[i + 2];
    uint32_t k3 = pk[i + 3];

    if ((bm[static_cast<std::size_t>(k0) >> 6] >> (k0 & 63u)) & 1ULL) sum += static_cast<uint64_t>(vals[i + 0]);
    if ((bm[static_cast<std::size_t>(k1) >> 6] >> (k1 & 63u)) & 1ULL) sum += static_cast<uint64_t>(vals[i + 1]);
    if ((bm[static_cast<std::size_t>(k2) >> 6] >> (k2 & 63u)) & 1ULL) sum += static_cast<uint64_t>(vals[i + 2]);
    if ((bm[static_cast<std::size_t>(k3) >> 6] >> (k3 & 63u)) & 1ULL) sum += static_cast<uint64_t>(vals[i + 3]);
  }
  for (; i < n; ++i) {
    uint32_t k = pk[i];
    if ((bm[static_cast<std::size_t>(k) >> 6] >> (k & 63u)) & 1ULL) {
      sum += static_cast<uint64_t>(vals[i]);
    }
  }

  return sum;
}
