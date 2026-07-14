#include "interface.h"
#include <roaring.h>
#include <vector>
#include <thread>
#include <algorithm>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  
  if (offsets.size() <= 1) {
    return 0;
  }
  
  const size_t num_shards = offsets.size() - 1;
  
  // For few huge shards, build bitmaps in parallel
  std::vector<roaring_bitmap_t*> bitmaps(num_shards);
  
  // Determine number of threads based on hardware and shard count
  unsigned int hw_threads = std::thread::hardware_concurrency();
  if (hw_threads == 0) hw_threads = 4;
  const size_t num_threads = std::min(static_cast<size_t>(hw_threads), num_shards);
  
  if (num_threads > 1 && num_shards > 1) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    auto worker = [&](size_t thread_id) {
      for (size_t shard = thread_id; shard < num_shards; shard += num_threads) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        const uint32_t count = end - begin;
        
        if (count == 0) {
          bitmaps[shard] = roaring_bitmap_create();
        } else {
          // Data is sorted, use optimized bulk add
          bitmaps[shard] = roaring_bitmap_of_ptr(count, values.data() + begin);
        }
      }
    };
    
    for (size_t t = 0; t < num_threads; ++t) {
      threads.emplace_back(worker, t);
    }
    
    for (auto& th : threads) {
      th.join();
    }
  } else {
    // Single-threaded for small number of shards
    for (size_t shard = 0; shard < num_shards; ++shard) {
      const uint32_t begin = offsets[shard];
      const uint32_t end = offsets[shard + 1];
      const uint32_t count = end - begin;
      
      if (count == 0) {
        bitmaps[shard] = roaring_bitmap_create();
      } else {
        bitmaps[shard] = roaring_bitmap_of_ptr(count, values.data() + begin);
      }
    }
  }
  
  // Use heap-based many-way union which is efficient for this use case
  roaring_bitmap_t* result = roaring_bitmap_or_many_heap(num_shards, const_cast<const roaring_bitmap_t**>(bitmaps.data()));
  
  uint64_t cardinality = roaring_bitmap_get_cardinality(result);
  
  // Cleanup
  roaring_bitmap_free(result);
  for (size_t i = 0; i < num_shards; ++i) {
    roaring_bitmap_free(bitmaps[i]);
  }
  
  return cardinality;
}