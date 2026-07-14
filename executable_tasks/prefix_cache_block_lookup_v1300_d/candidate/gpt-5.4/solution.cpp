#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

static inline uint32_t smear32(uint32_t x) {
  x ^= x >> 16;
  x *= 0x7feb352dU;
  x ^= x >> 15;
  x *= 0x846ca68bU;
  x ^= x >> 16;
  return x;
}

class MembershipTable {
 public:
  MembershipTable() : mask_(0), empty_key_(0) {}

  explicit MembershipTable(const std::vector<uint32_t>& keys) { build(keys); }

  void build(const std::vector<uint32_t>& keys) {
    const std::size_t n = keys.size();
    if (n == 0) {
      table_.clear();
      mask_ = 0;
      empty_key_ = 0;
      return;
    }

    std::size_t cap = 1;
    while (cap < n * 2) cap <<= 1;
    if (cap < 8) cap = 8;

    table_.assign(cap, kEmpty);
    mask_ = static_cast<uint32_t>(cap - 1);

    bool has_empty = false;
    for (uint32_t v : keys) {
      if (v == kEmpty) {
        has_empty = true;
        continue;
      }
      uint32_t idx = smear32(v) & mask_;
      while (table_[idx] != kEmpty && table_[idx] != v) {
        idx = (idx + 1) & mask_;
      }
      table_[idx] = v;
    }
    empty_key_ = has_empty ? 1u : 0u;
  }

  inline bool contains(uint32_t v) const {
    if (table_.empty()) return false;
    if (v == kEmpty) return empty_key_ != 0;
    uint32_t idx = smear32(v) & mask_;
    for (;;) {
      const uint32_t cur = table_[idx];
      if (cur == kEmpty) return false;
      if (cur == v) return true;
      idx = (idx + 1) & mask_;
    }
  }

 private:
  static constexpr uint32_t kEmpty = 0xFFFFFFFFu;
  std::vector<uint32_t> table_;
  uint32_t mask_;
  uint32_t empty_key_;
};

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;

  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (iters <= 0) return 0;
  if (num_requests == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  MembershipTable cache(sorted_cache_keys);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int begin = request_offsets[r];
      const int end = request_offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = begin; i < end; ++i) {
        if (!cache.contains(request_blocks[static_cast<std::size_t>(i)])) break;
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}
