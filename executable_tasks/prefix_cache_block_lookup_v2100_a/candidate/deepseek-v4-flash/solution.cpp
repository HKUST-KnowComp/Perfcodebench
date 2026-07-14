#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstddef>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Simple open-addressing hash set with power-of-two size
class FastSet {
 public:
  explicit FastSet(const std::vector<uint32_t>& keys) {
    std::size_t cap = 8;
    while (cap < 2 * keys.size()) cap <<= 1;
    mask_ = cap - 1;
    keys_.assign(cap, UINT32_MAX);
    occupied_.assign(cap, 0);
    for (auto k : keys) {
      std::size_t idx = k & mask_;
      while (occupied_[idx]) {
        idx = (idx + 1) & mask_;
      }
      keys_[idx] = k;
      occupied_[idx] = 1;
    }
  }

  bool contains(uint32_t key) const noexcept {
    std::size_t idx = key & mask_;
    while (occupied_[idx]) {
      if (keys_[idx] == key) return true;
      idx = (idx + 1) & mask_;
    }
    return false;
  }

 private:
  std::vector<uint32_t> keys_;
  std::vector<char> occupied_;
  std::size_t mask_;
};

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  FastSet cache_set(sorted_cache_keys);

  uint64_t hash = 0;
  const std::size_t num_requests = request_offsets.size() - 1;
  const auto* offsets = request_offsets.data();
  const auto* blocks = request_blocks.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      std::size_t start = static_cast<std::size_t>(offsets[r]);
      std::size_t end = static_cast<std::size_t>(offsets[r + 1]);
      uint64_t prefix = 0;
      for (std::size_t i = start; i < end; ++i) {
        if (!cache_set.contains(blocks[i])) break;
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}