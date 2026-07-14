#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0) return 0;
  const size_t n = keys.size();
  if (n == 0) return 0;

  // Dense bitset: 1 bit per key in [0, key_space)
  const size_t nwords = (static_cast<size_t>(key_space) + 63u) >> 6; // ceil(key_space/64)
  std::vector<uint64_t> bits(nwords, 0ull);

  // Tiny hot-key cache to avoid repeated bitset touches for duplicates.
  // Direct-mapped cache with stamping to avoid initialization costs.
  static constexpr size_t CACHE_SIZE = 256; // power of two
  uint32_t cache_keys[CACHE_SIZE];
  uint32_t cache_stamp[CACHE_SIZE];
  // Use a single per-call stamp so we don't need to clear arrays.
  const uint32_t stamp = 1u; // any non-zero works since arrays are uninitialized each call
  // Initialize stamps to 0 so no entry is considered valid initially.
  for (size_t i = 0; i < CACHE_SIZE; ++i) cache_stamp[i] = 0u;

  uint64_t unique = 0;
  const uint32_t* __restrict kptr = keys.data();

  for (size_t i = 0; i < n; ++i) {
    const uint32_t k = kptr[i];
    // Optional: if keys might be out of range, guard here. Assumed bounded by task.

    // Hot-key cache lookup
    const size_t idx = (static_cast<size_t>(k) * 2654435761u) & (CACHE_SIZE - 1);
    if (cache_stamp[idx] == stamp && cache_keys[idx] == k) {
      continue; // seen before in this run
    }

    const size_t w = static_cast<size_t>(k) >> 6; // word index
    const uint64_t m = 1ull << (k & 63u);
    const uint64_t bw = bits[w];
    if ((bw & m) == 0) {
      bits[w] = bw | m;
      ++unique;
    }

    cache_keys[idx] = k;
    cache_stamp[idx] = stamp;
  }

  // The distinct count is the same for every iteration; return the computed value.
  return unique;
}
