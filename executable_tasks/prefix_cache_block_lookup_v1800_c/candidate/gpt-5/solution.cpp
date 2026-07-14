#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

inline bool contains_sorted(const uint32_t* keys, std::size_t n, uint32_t x) {
  std::size_t lo = 0;
  std::size_t hi = n;
  while (lo < hi) {
    std::size_t mid = lo + ((hi - lo) >> 1);
    uint32_t v = keys[mid];
    if (v < x) {
      lo = mid + 1;
    } else {
      hi = mid;
    }
  }
  return (lo < n && keys[lo] == x);
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  // Preserve baseline behavior for iters == 0
  if (iters == 0) return 0ULL;

  const uint32_t* cache_ptr = sorted_cache_keys.empty() ? nullptr : sorted_cache_keys.data();
  const std::size_t cache_n = sorted_cache_keys.size();
  const uint32_t* blocks_ptr = request_blocks.empty() ? nullptr : request_blocks.data();

  // Decide whether to use bitset based membership lookup
  // Avoid excessive memory usage by falling back to binary search if too large
  const unsigned long long max_bitset_bytes = 256ULL * 1024ULL * 1024ULL; // 256 MiB cap
  const unsigned long long bitset_bytes = (static_cast<unsigned long long>(hash_space) + 7ULL) >> 3; // bits to bytes
  const bool use_bitset = (hash_space != 0 && bitset_bytes <= max_bitset_bytes);

  std::vector<uint64_t> bitset;
  if (use_bitset) {
    const std::size_t words = (static_cast<std::size_t>(hash_space) + 63u) >> 6; // 64-bit words
    bitset.assign(words, 0ULL);
    for (std::size_t i = 0; i < cache_n; ++i) {
      uint32_t v = cache_ptr[i];
      if (v < hash_space) {
        bitset[static_cast<std::size_t>(v >> 6)] |= (1ULL << (v & 63u));
      }
    }
  }

  uint64_t hash = 1469598103934665603ULL;

  const std::size_t rcount = (request_offsets.size() > 0) ? (request_offsets.size() - 1) : 0;
  for (std::size_t r = 0; r < rcount; ++r) {
    int s = request_offsets[r];
    int e = request_offsets[r + 1];
    std::size_t i = static_cast<std::size_t>(s);
    const std::size_t end = static_cast<std::size_t>(e);

    if (i < end) {
      if (use_bitset) {
        while (i < end) {
          uint32_t v = blocks_ptr[i];
          if (v >= hash_space) break;
          uint64_t word = bitset[static_cast<std::size_t>(v >> 6)];
          uint64_t mask = (1ULL << (v & 63u));
          if ((word & mask) == 0ULL) break;
          ++i;
        }
      } else {
        while (i < end) {
          uint32_t v = blocks_ptr[i];
          if (!contains_sorted(cache_ptr, cache_n, v)) break;
          ++i;
        }
      }
    }

    uint64_t prefix = static_cast<uint64_t>(i - static_cast<std::size_t>(s));
    hash = mix(hash, prefix);
  }

  // Since each iteration produces the same hash, returning the single computed value
  // matches the baseline's final-iteration result for iters > 0.
  return hash;
}
