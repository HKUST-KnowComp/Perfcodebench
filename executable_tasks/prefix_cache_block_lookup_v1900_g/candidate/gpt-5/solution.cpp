#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

// FNV-1a style mix retained from baseline
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Lightweight open-addressing hash set for uint32_t keys
class Uint32HashSet {
 public:
  Uint32HashSet() : mask_(0) {}

  void build(const std::vector<uint32_t>& keys_in) {
    const std::size_t n = keys_in.size();
    // Choose capacity as power of two >= 2*n (load factor <= 0.5 for speed)
    std::size_t cap = 1;
    while (cap < (n << 1)) cap <<= 1;
    if (cap == 0) cap = 1;  // safety for n == 0 overflow (though unlikely)
    keys_.assign(cap, 0u);
    used_.assign(cap, 0u);
    mask_ = static_cast<uint32_t>(cap - 1);

    for (uint32_t k : keys_in) {
      insert(k);
    }
  }

  inline bool contains(uint32_t key) const {
    if (used_.empty()) return false;
    uint32_t idx = hash(key) & mask_;
    // Linear probing; stop on first empty slot
    while (used_[idx]) {
      if (keys_[idx] == key) return true;
      idx = (idx + 1) & mask_;
    }
    return false;
  }

 private:
  inline void insert(uint32_t key) {
    uint32_t idx = hash(key) & mask_;
    while (used_[idx]) {
      if (keys_[idx] == key) return;  // avoid duplicates (input is sorted but safe)
      idx = (idx + 1) & mask_;
    }
    used_[idx] = 1;
    keys_[idx] = key;
  }

  // 32-bit mix to distribute lower bits well for power-of-two tables
  static inline uint32_t hash(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
  }

  std::vector<uint32_t> keys_;
  std::vector<uint8_t> used_;
  uint32_t mask_;
};

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t /*hash_space*/,
    int iters) {
  // Build hash set once for fast membership checks
  Uint32HashSet set;
  set.build(sorted_cache_keys);

  uint64_t hash = 0;
  const std::size_t num_reqs = (request_offsets.size() > 0) ? (request_offsets.size() - 1) : 0;
  const uint32_t* blocks = request_blocks.data();
  const int* offs = request_offsets.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV offset basis
    for (std::size_t r = 0; r < num_reqs; ++r) {
      uint64_t prefix = 0;
      int start = offs[r];
      int end = offs[r + 1];
      // Iterate through the blocks of this request until a miss
      for (int i = start; i < end; ++i) {
        uint32_t key = blocks[static_cast<std::size_t>(i)];
        if (!set.contains(key)) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}
