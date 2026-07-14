#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

class HashSet {
public:
  HashSet(const std::vector<uint32_t>& keys, uint32_t hash_space) {
    size_t n = keys.size();
    capacity_ = 1;
    while (capacity_ < n * 2) {
      capacity_ *= 2;
    }
    mask_ = capacity_ - 1;
    table_.resize(capacity_, UINT32_MAX);
    
    for (uint32_t key : keys) {
      uint32_t idx = (key * 2654435761U) & mask_;
      while (table_[idx] != UINT32_MAX) {
        idx = (idx + 1) & mask_;
      }
      table_[idx] = key;
    }
  }
  
  inline bool contains(uint32_t key) const {
    uint32_t idx = (key * 2654435761U) & mask_;
    while (true) {
      uint32_t stored = table_[idx];
      if (stored == key) return true;
      if (stored == UINT32_MAX) return false;
      idx = (idx + 1) & mask_;
    }
  }
  
private:
  std::vector<uint32_t> table_;
  uint32_t capacity_;
  uint32_t mask_;
};

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  HashSet cache_set(sorted_cache_keys, hash_space);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      for (int i = start; i < end; ++i) {
        if (!cache_set.contains(request_blocks[static_cast<std::size_t>(i)])) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}