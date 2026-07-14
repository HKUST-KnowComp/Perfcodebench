#include "interface.h"

#include <roaring.h>

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (shard_count == 0 || values.empty()) {
    return 0;
  }

  // For the small-total-size latency regime, a direct k-way merge over the
  // already-sorted shards avoids bitmap construction overhead and computes the
  // exact union cardinality in a single pass.
  std::vector<uint32_t> pos(shard_count);
  std::vector<uint32_t> end(shard_count);

  bool any_nonempty = false;
  for (std::size_t s = 0; s < shard_count; ++s) {
    uint32_t b = offsets[s];
    uint32_t e = offsets[s + 1];
    if (b > values.size()) b = static_cast<uint32_t>(values.size());
    if (e > values.size()) e = static_cast<uint32_t>(values.size());
    if (e < b) e = b;
    pos[s] = b;
    end[s] = e;
    any_nonempty |= (b < e);
  }
  if (!any_nonempty) {
    return 0;
  }

  uint64_t count = 0;
  bool have_last = false;
  uint32_t last = 0;

  while (true) {
    uint32_t min_value = 0;
    bool found = false;

    for (std::size_t s = 0; s < shard_count; ++s) {
      if (pos[s] < end[s]) {
        const uint32_t v = values[pos[s]];
        if (!found || v < min_value) {
          min_value = v;
          found = true;
        }
      }
    }

    if (!found) {
      break;
    }

    if (!have_last || min_value != last) {
      ++count;
      last = min_value;
      have_last = true;
    }

    for (std::size_t s = 0; s < shard_count; ++s) {
      uint32_t p = pos[s];
      const uint32_t e = end[s];
      while (p < e && values[p] == min_value) {
        ++p;
      }
      pos[s] = p;
    }
  }

  return count;
}
