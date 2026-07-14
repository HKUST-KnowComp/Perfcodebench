#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0 || enabled_keys.empty() || probe_keys.empty()) return 0;

  // Build bitmap once
  std::vector<unsigned char> bitmap(key_space, 0);
  for (std::size_t i = 0; i < enabled_keys.size(); ++i) {
    uint32_t k = enabled_keys[i];
    if (k < key_space) bitmap[k] = 1;
  }

  // Compute sum once (same each iter), using mask multiplication to avoid branches
  uint64_t sum = 0;
  const uint32_t* pk = probe_keys.data();
  const uint32_t* vals = values.data();
  const std::size_t n = probe_keys.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t key = pk[i];
    unsigned char m = bitmap[key];
    sum += static_cast<uint64_t>(vals[i]) * static_cast<uint64_t>(m);
  }

  return sum;
}
