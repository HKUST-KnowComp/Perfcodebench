#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  const std::size_t ln = left_keys.size();
  const std::size_t rn = right_keys.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    std::size_t i = 0;
    std::size_t j = 0;

    while (i < ln && j < rn) {
      const uint32_t lk = left_keys[i];
      const uint32_t rk = right_keys[j];

      if (lk < rk) {
        std::size_t ni = i + 1;
        while (ni < ln && left_keys[ni] == lk) ++ni;
        h = mix(h, static_cast<uint64_t>(lk));
        h = mix(h, static_cast<uint64_t>(left_values[ni - 1]));
        i = ni;
      } else if (rk < lk) {
        std::size_t nj = j + 1;
        while (nj < rn && right_keys[nj] == rk) ++nj;
        h = mix(h, static_cast<uint64_t>(rk));
        h = mix(h, static_cast<uint64_t>(right_values[nj - 1]));
        j = nj;
      } else {
        const uint32_t k = lk;
        std::size_t ni = i + 1;
        while (ni < ln && left_keys[ni] == k) ++ni;
        std::size_t nj = j + 1;
        while (nj < rn && right_keys[nj] == k) ++nj;
        h = mix(h, static_cast<uint64_t>(k));
        h = mix(h, static_cast<uint64_t>(right_values[nj - 1]));
        i = ni;
        j = nj;
      }
    }

    while (i < ln) {
      const uint32_t k = left_keys[i];
      std::size_t ni = i + 1;
      while (ni < ln && left_keys[ni] == k) ++ni;
      h = mix(h, static_cast<uint64_t>(k));
      h = mix(h, static_cast<uint64_t>(left_values[ni - 1]));
      i = ni;
    }

    while (j < rn) {
      const uint32_t k = right_keys[j];
      std::size_t nj = j + 1;
      while (nj < rn && right_keys[nj] == k) ++nj;
      h = mix(h, static_cast<uint64_t>(k));
      h = mix(h, static_cast<uint64_t>(right_values[nj - 1]));
      j = nj;
    }

    hash = h;
  }

  return hash;
}
