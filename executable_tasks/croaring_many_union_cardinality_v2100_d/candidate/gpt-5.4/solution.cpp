#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const std::size_t shard_count = offsets.size();
  if (shard_count <= 1) return 0;

  struct Cursor {
    const uint32_t* ptr;
    const uint32_t* end;
  };

  std::vector<Cursor> cursors;
  cursors.reserve(shard_count - 1);

  for (std::size_t shard = 0; shard + 1 < shard_count; ++shard) {
    uint32_t begin = offsets[shard];
    uint32_t end = offsets[shard + 1];
    if (begin < end) {
      cursors.push_back(Cursor{values.data() + begin, values.data() + end});
    }
  }

  if (cursors.empty()) return 0;

  uint64_t cardinality = 0;

  while (true) {
    uint32_t min_value = cursors[0].ptr[0];
    for (std::size_t i = 1; i < cursors.size(); ++i) {
      const uint32_t v = cursors[i].ptr[0];
      if (v < min_value) min_value = v;
    }

    ++cardinality;

    std::size_t write = 0;
    for (std::size_t i = 0; i < cursors.size(); ++i) {
      const uint32_t* p = cursors[i].ptr;
      const uint32_t* e = cursors[i].end;
      while (p != e && *p == min_value) {
        ++p;
      }
      if (p != e) {
        cursors[write++] = Cursor{p, e};
      }
    }
    cursors.resize(write);
    if (cursors.empty()) break;
  }

  return cardinality;
}
