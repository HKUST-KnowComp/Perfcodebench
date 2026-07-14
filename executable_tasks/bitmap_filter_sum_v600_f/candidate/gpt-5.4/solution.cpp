#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0 || probe_keys.empty() || values.empty() || key_space == 0) {
    return 0;
  }

  const std::size_t n = probe_keys.size();
  const std::size_t m = values.size();
  const std::size_t count = n < m ? n : m;

  std::vector<uint32_t> marks(static_cast<std::size_t>(key_space), 0u);
  uint32_t gen = 1u;
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (gen == 0u) {
      for (std::size_t i = 0; i < marks.size(); ++i) marks[i] = 0u;
      gen = 1u;
    }

    for (std::size_t i = 0; i < enabled_keys.size(); ++i) {
      const uint32_t k = enabled_keys[i];
      if (k < key_space) {
        marks[k] = gen;
      }
    }

    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < count; ++i) {
      const uint32_t k = probe_keys[i];
      if (k < key_space && marks[k] == gen) {
        local_sum += static_cast<uint64_t>(values[i]);
      }
    }

    sum = local_sum;
    ++gen;
  }

  return sum;
}
