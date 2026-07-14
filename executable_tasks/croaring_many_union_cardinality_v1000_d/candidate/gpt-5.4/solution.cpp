#include "interface.h"

#include <roaring.h>

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (shard_count == 0) return 0;

  // Fast path for a single shard: input shards are sorted, so cardinality is
  // just the number of unique adjacent values in that shard.
  if (shard_count == 1) {
    const uint32_t begin = offsets[0];
    const uint32_t end = offsets[1];
    if (begin >= end) return 0;
    uint64_t count = 1;
    uint32_t prev = values[begin];
    for (uint32_t i = begin + 1; i < end; ++i) {
      const uint32_t v = values[i];
      if (v != prev) {
        ++count;
        prev = v;
      }
    }
    return count;
  }

  // For a few huge sorted shards, a k-way merge over the shard heads avoids
  // materializing large intermediate bitmaps and is typically faster than
  // hash-based deduplication. Duplicates within and across shards are skipped
  // as we advance.
  std::vector<uint32_t> pos(shard_count);
  std::vector<uint32_t> end(shard_count);
  std::vector<uint32_t> active;
  active.reserve(shard_count);

  for (std::size_t s = 0; s < shard_count; ++s) {
    uint32_t b = offsets[s];
    uint32_t e = offsets[s + 1];
    pos[s] = b;
    end[s] = e;
    if (b < e) active.push_back(static_cast<uint32_t>(s));
  }

  if (active.empty()) return 0;

  uint64_t cardinality = 0;
  bool have_last = false;
  uint32_t last = 0;

  while (!active.empty()) {
    uint32_t min_value = values[pos[active[0]]];
    for (std::size_t i = 1; i < active.size(); ++i) {
      const uint32_t v = values[pos[active[i]]];
      if (v < min_value) min_value = v;
    }

    if (!have_last || min_value != last) {
      ++cardinality;
      last = min_value;
      have_last = true;
    }

    std::size_t write = 0;
    for (std::size_t i = 0; i < active.size(); ++i) {
      const uint32_t s = active[i];
      uint32_t p = pos[s];
      const uint32_t e = end[s];
      if (values[p] == min_value) {
        do {
          ++p;
        } while (p < e && values[p] == min_value);
        pos[s] = p;
      }
      if (pos[s] < e) {
        active[write++] = s;
      }
    }
    active.resize(write);
  }

  return cardinality;
}
