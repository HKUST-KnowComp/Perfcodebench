#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size() > 0 ? offsets.size() - 1 : 0;
  if (shard_count == 0) return 0;

  struct Cursor {
    const uint32_t* ptr;
    const uint32_t* end;
  };

  std::vector<Cursor> cursors;
  cursors.reserve(shard_count);
  for (std::size_t s = 0; s < shard_count; ++s) {
    const uint32_t begin = offsets[s];
    const uint32_t end = offsets[s + 1];
    cursors.push_back(Cursor{values.data() + begin, values.data() + end});
  }

  uint64_t count = 0;
  bool have_last = false;
  uint32_t last = 0;

  while (true) {
    uint32_t min_value = 0;
    bool found = false;

    for (std::size_t i = 0; i < shard_count; ++i) {
      if (cursors[i].ptr != cursors[i].end) {
        const uint32_t v = *cursors[i].ptr;
        if (!found || v < min_value) {
          min_value = v;
          found = true;
        }
      }
    }

    if (!found) break;

    if (!have_last || min_value != last) {
      ++count;
      last = min_value;
      have_last = true;
    }

    for (std::size_t i = 0; i < shard_count; ++i) {
      const uint32_t* p = cursors[i].ptr;
      const uint32_t* const e = cursors[i].end;
      while (p != e && *p == min_value) {
        ++p;
      }
      cursors[i].ptr = p;
    }
  }

  return count;
}
