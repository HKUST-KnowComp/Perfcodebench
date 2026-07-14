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
  
  const size_t ln = left_keys.size();
  const size_t rn = right_keys.size();
  
  const uint32_t* lk = ln > 0 ? left_keys.data() : nullptr;
  const uint32_t* lv = ln > 0 ? left_values.data() : nullptr;
  const uint32_t* rk = rn > 0 ? right_keys.data() : nullptr;
  const uint32_t* rv = rn > 0 ? right_values.data() : nullptr;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t li = 0;
    size_t ri = 0;
    uint32_t last_key = 0;
    bool has_last = false;
    uint32_t last_val = 0;
    
    while (li < ln && ri < rn) {
      uint32_t lkey = lk[li];
      uint32_t rkey = rk[ri];
      
      if (lkey < rkey) {
        // Emit from left
        if (has_last && last_key == lkey) {
          // duplicate with pending, left comes first so right (pending) wins - skip left
          // Actually left is current, last is from previous. If last_key == lkey, 
          // we need to see which side last came from.
          // Since we process in sorted order and right wins on ties,
          // if last_key == lkey, last was set from a previous element.
          // But lkey < rkey, so this left element has same key as last.
          // We should update last_val only if this is "later" - but left comes before right
          // in stable order. So if last was from left, update. If last was from right, keep right.
          // This is getting complex. Let me reconsider.
          // 
          // Actually, the original stable_sort puts left before right for same keys.
          // Then dedup keeps the LAST occurrence. So for same key:
          // - if both left and right have it, right wins (appears after left in stable sort)
          // - if only left has it, left wins
          // - if only right has it, right wins
          //
          // So in merge: when keys equal, we should take right's value.
          // When processing, we need to handle duplicates within each side too.
          //
          // Let me simplify: collect all (key,val) pairs, then for each unique key output the last value.
          // With sorted inputs, I can do this by tracking the "current" key and updating value,
          // then emitting when key changes.
          last_val = lv[li];
        } else {
          // New key or first key
          if (has_last) {
            hash = mix(hash, last_key);
            hash = mix(hash, last_val);
          }
          last_key = lkey;
          last_val = lv[li];
          has_last = true;
        }
        ++li;
      } else if (rkey < lkey) {
        // Emit from right
        if (has_last && last_key == rkey) {
          last_val = rv[ri];
        } else {
          if (has_last) {
            hash = mix(hash, last_key);
            hash = mix(hash, last_val);
          }
          last_key = rkey;
          last_val = rv[ri];
          has_last = true;
        }
        ++ri;
      } else {
        // lkey == rkey, process left first (stable), then right
        if (has_last && last_key == lkey) {
          last_val = lv[li];
        } else {
          if (has_last) {
            hash = mix(hash, last_key);
            hash = mix(hash, last_val);
          }
          last_key = lkey;
          last_val = lv[li];
          has_last = true;
        }
        ++li;
        // Now process right with same key - right overwrites
        last_val = rv[ri];
        ++ri;
      }
    }
    
    // Process remaining left
    while (li < ln) {
      uint32_t lkey = lk[li];
      if (has_last && last_key == lkey) {
        last_val = lv[li];
      } else {
        if (has_last) {
          hash = mix(hash, last_key);
          hash = mix(hash, last_val);
        }
        last_key = lkey;
        last_val = lv[li];
        has_last = true;
      }
      ++li;
    }
    
    // Process remaining right
    while (ri < rn) {
      uint32_t rkey = rk[ri];
      if (has_last && last_key == rkey) {
        last_val = rv[ri];
      } else {
        if (has_last) {
          hash = mix(hash, last_key);
          hash = mix(hash, last_val);
        }
        last_key = rkey;
        last_val = rv[ri];
        has_last = true;
      }
      ++ri;
    }
    
    // Emit final pending key
    if (has_last) {
      hash = mix(hash, last_key);
      hash = mix(hash, last_val);
    }
  }
  
  return hash;
}