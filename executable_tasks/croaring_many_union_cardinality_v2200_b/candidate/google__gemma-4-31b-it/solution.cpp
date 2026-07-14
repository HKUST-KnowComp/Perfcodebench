#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  if (offsets.empty()) return 0;

  croaring::Roaring bitmap;
  
  // Pre-calculate total elements to potentially optimize bitmap allocation
  // though CRoaring handles dynamic growth efficiently.
  for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
    const uint32_t begin = offsets[shard];
    const uint32_t end = offsets[shard + 1];
    
    // Use the bulk add method for better performance
    if (begin < end) {
      bitmap.add_range(values.data() + begin, end - begin);
    }
  }
  
  return bitmap.cardinality();
}