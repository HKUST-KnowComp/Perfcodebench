#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  const size_t left_size = left_keys.size();
  const size_t right_size = right_keys.size();
  
  const uint32_t* __restrict__ lk = left_keys.data();
  const uint32_t* __restrict__ lv = left_values.data();
  const uint32_t* __restrict__ rk = right_keys.data();
  const uint32_t* __restrict__ rv = right_values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;  // left index
    size_t j = 0;  // right index
    
    // Track the last key we output to handle duplicates
    uint32_t last_key = 0;
    bool has_last = false;
    uint32_t pending_value = 0;
    
    while (i < left_size && j < right_size) {
      uint32_t lkey = lk[i];
      uint32_t rkey = rk[j];
      
      if (lkey < rkey) {
        // Left key is smaller
        // Check if this key exists in right (would be overwritten)
        // Since right is sorted, if rkey > lkey, lkey won't appear later in right
        // But we need to check if lkey equals a future right key
        // Actually, since right is sorted and rkey > lkey, lkey can't be in right
        // So we can output left
        if (has_last && last_key == lkey) {
          // Update pending value (left overwrites previous left with same key)
          pending_value = lv[i];
        } else {
          // Flush pending if any
          if (has_last) {
            hash = mix(hash, last_key);
            hash = mix(hash, pending_value);
          }
          last_key = lkey;
          pending_value = lv[i];
          has_last = true;
        }
        ++i;
      } else if (rkey < lkey) {
        // Right key is smaller
        if (has_last && last_key == rkey) {
          pending_value = rv[j];
        } else {
          if (has_last) {
            hash = mix(hash, last_key);
            hash = mix(hash, pending_value);
          }
          last_key = rkey;
          pending_value = rv[j];
          has_last = true;
        }
        ++j;
      } else {
        // Keys are equal - right wins (keep last semantics with right after left)
        // But we process left first, then right overwrites
        if (has_last && last_key == lkey) {
          // Same key as pending, right will overwrite
          pending_value = rv[j];
        } else {
          if (has_last) {
            hash = mix(hash, last_key);
            hash = mix(hash, pending_value);
          }
          last_key = lkey;
          pending_value = rv[j];  // Right value wins
          has_last = true;
        }
        ++i;
        ++j;
      }
    }
    
    // Process remaining left
    while (i < left_size) {
      uint32_t lkey = lk[i];
      if (has_last && last_key == lkey) {
        pending_value = lv[i];
      } else {
        if (has_last) {
          hash = mix(hash, last_key);
          hash = mix(hash, pending_value);
        }
        last_key = lkey;
        pending_value = lv[i];
        has_last = true;
      }
      ++i;
    }
    
    // Process remaining right
    while (j < right_size) {
      uint32_t rkey = rk[j];
      if (has_last && last_key == rkey) {
        pending_value = rv[j];
      } else {
        if (has_last) {
          hash = mix(hash, last_key);
          hash = mix(hash, pending_value);
        }
        last_key = rkey;
        pending_value = rv[j];
        has_last = true;
      }
      ++j;
    }
    
    // Flush final pending
    if (has_last) {
      hash = mix(hash, last_key);
      hash = mix(hash, pending_value);
    }
  }
  
  return hash;
}