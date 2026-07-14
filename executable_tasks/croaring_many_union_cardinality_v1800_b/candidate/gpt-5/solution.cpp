#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t shard_count = offsets.size() > 0 ? (offsets.size() - 1) : 0;
  if (shard_count == 0) return 0;

  const uint32_t* base = values.data();

  // Fast path for single shard: count unique in a sorted array
  if (shard_count == 1) {
    const uint32_t* cur = base + offsets[0];
    const uint32_t* end = base + offsets[1];
    if (cur == end) return 0;
    uint64_t count = 0;
    uint32_t prev = 0;
    bool have_prev = false;
    while (cur != end) {
      uint32_t v = *cur++;
      if (!have_prev || v != prev) {
        prev = v;
        have_prev = true;
        ++count;
      }
    }
    return count;
  }

  // General path: multi-way merge across few large shards.
  std::vector<const uint32_t*> curs(shard_count);
  std::vector<const uint32_t*> ends(shard_count);
  for (size_t i = 0; i < shard_count; ++i) {
    curs[i] = base + offsets[i];
    ends[i] = base + offsets[i + 1];
  }

  uint64_t count = 0;
  for (;;) {
    const uint32_t* min_ptr = nullptr;
    // Find current minimum across all non-empty shards
    for (size_t i = 0; i < shard_count; ++i) {
      const uint32_t* c = curs[i];
      if (c != ends[i]) {
        if (!min_ptr || *c < *min_ptr) {
          min_ptr = c;
        }
      }
    }
    if (!min_ptr) break; // all shards exhausted

    const uint32_t m = *min_ptr;

    // Advance all shards past value m (also collapses duplicates within a shard)
    for (size_t i = 0; i < shard_count; ++i) {
      const uint32_t* c = curs[i];
      const uint32_t* e = ends[i];
      while (c != e) {
        uint32_t v = *c;
        if (v != m) break;
        ++c;
      }
      curs[i] = c;
    }

    ++count;
  }

  return count;
}
