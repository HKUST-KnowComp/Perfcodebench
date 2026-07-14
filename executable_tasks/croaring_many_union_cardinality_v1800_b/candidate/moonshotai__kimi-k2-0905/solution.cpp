#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t n_shards = offsets.empty() ? 0 : offsets.size() - 1;
  if (n_shards == 0) return 0;

  // Build one roaring bitmap per shard
  roaring_bitmap_t** shards =
      static_cast<roaring_bitmap_t**>(malloc(n_shards * sizeof(roaring_bitmap_t*)));
  if (!shards) return 0;

  for (size_t shard = 0; shard < n_shards; ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end   = offsets[shard + 1];
    shards[shard] = roaring_bitmap_of_ptr(end - begin, values.data() + begin);
  }

  // Compute the big union
  roaring_bitmap_t* union_bitmap = roaring_bitmap_or_many_heap(n_shards, shards);
  const uint64_t cardinality = roaring_bitmap_get_cardinality(union_bitmap);

  // Cleanup
  roaring_bitmap_free(union_bitmap);
  for (size_t shard = 0; shard < n_shards; ++shard) {
    roaring_bitmap_free(shards[shard]);
  }
  free(shards);

  return cardinality;
}