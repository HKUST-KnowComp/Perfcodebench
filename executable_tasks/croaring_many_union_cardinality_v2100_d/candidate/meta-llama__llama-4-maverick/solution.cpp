#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  Roaring r;
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    Roaring shard_bitmap;
    shard_bitmap.addRange(begin, end);
    r.orWith(shard_bitmap);
    r.removeRange(begin, end);
    shard_bitmap.addMany(end - begin, values.data() + begin);
    r.orWith(shard_bitmap);
  }
  return r.cardinality();
}