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

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  // Build a compact bitset for O(1) membership checks.
  const uint32_t hs = hash_space;
  const std::size_t num_words = (static_cast<std::size_t>(hs) + 63u) >> 6; // ceil(hs/64)
  std::vector<uint64_t> bits(num_words, 0);
  uint64_t* bits_ptr = bits.empty() ? nullptr : bits.data();

  for (uint32_t key : sorted_cache_keys) {
    if (key < hs) {
      const std::size_t idx = static_cast<std::size_t>(key) >> 6;
      const uint32_t off = key & 63u;
      bits_ptr[idx] |= (1ULL << off);
    }
  }

  // Precompute per-request prefix lengths once.
  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  std::vector<uint32_t> prefix_lengths;
  prefix_lengths.resize(num_requests);

  const uint32_t* req_blocks_ptr = request_blocks.empty() ? nullptr : request_blocks.data();
  const int* offsets_ptr = request_offsets.empty() ? nullptr : request_offsets.data();

  for (std::size_t r = 0; r < num_requests; ++r) {
    const int start = offsets_ptr[r];
    const int end = offsets_ptr[r + 1];
    uint32_t cnt = 0;
    for (int i = start; i < end; ++i) {
      const uint32_t b = req_blocks_ptr[static_cast<std::size_t>(i)];
      bool present = false;
      if (b < hs) {
        const std::size_t widx = static_cast<std::size_t>(b) >> 6;
        const uint32_t off = b & 63u;
        present = ((bits_ptr[widx] >> off) & 1ULL) != 0ULL;
      }
      if (!present) {
        break;
      }
      ++cnt;
    }
    prefix_lengths[r] = cnt;
  }

  // Compute the checksum for the specified number of iterations.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV offset basis
    for (std::size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, static_cast<uint64_t>(prefix_lengths[r]));
    }
  }
  return hash;
}
