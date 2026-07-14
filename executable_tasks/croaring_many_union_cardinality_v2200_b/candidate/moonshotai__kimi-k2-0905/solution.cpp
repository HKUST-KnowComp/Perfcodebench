#include "interface.h"
#include "roaring.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  const size_t n_shards = offsets.size() - 1;
  if (n_shards == 0) return 0;

  // Build one small bitmap per shard in parallel
  std::vector<roaring_bitmap_t*> shards;
  shards.reserve(n_shards);

  #pragma omp parallel for schedule(dynamic, 1)
  for (size_t s = 0; s < n_shards; ++s) {
    const uint32_t begin = offsets[s];
    const uint32_t end   = offsets[s + 1];
    roaring_bitmap_t* r  = roaring_bitmap_create();
    roaring_bitmap_add_range(r, begin, end);
    for (uint32_t i = begin; i < end; ++i) {
      roaring_bitmap_add(r, values[i]);
    }
    #pragma omp critical
    shards.push_back(r);
  }

  // Fast many-way union via Roaring's heap algorithm
  roaring_bitmap_t* uni = roaring_bitmap_or_many_heap(shards.size(), shards.data());
  const uint64_t card   = roaring_bitmap_get_cardinality(uni);

  // Cleanup
  roaring_bitmap_free(uni);
  for (auto* r : shards) roaring_bitmap_free(r);

  return card;
}