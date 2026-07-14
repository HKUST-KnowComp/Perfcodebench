#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (keys.empty() || key_space == 0u) return 0;

  std::vector<uint32_t> stamps;
  stamps.resize(static_cast<size_t>(key_space), 0u);

  uint32_t gen = 1u; // current generation token (non-zero)
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Advance generation; on wrap, clear stamps and restart generation
    gen += 1u;
    if (gen == 0u) {
      // Clear stamps to 0
      for (size_t i = 0, n = stamps.size(); i < n; ++i) stamps[i] = 0u;
      gen = 2u; // 1 is reserved initial; start from 2 for this iteration
    }

    uint64_t c = 0;
    uint32_t* stamp_ptr = stamps.data();
    const size_t n = keys.size();
    const uint32_t space = key_space;

    for (size_t i = 0; i < n; ++i) {
      uint32_t k = keys[i];
      if (k < space) {
        if (stamp_ptr[k] != gen) {
          stamp_ptr[k] = gen;
          ++c;
        }
      }
    }

    count = c; // keep the last iteration's result
  }

  return count;
}
