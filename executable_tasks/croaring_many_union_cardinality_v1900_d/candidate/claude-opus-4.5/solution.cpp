#include "interface.h"

#include <roaring.h>
#include <thread>
#include <vector>
#include <algorithm>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
  
  if (offsets.size() <= 1 || values.empty()) {
    return 0;
  }
  
  const size_t total_values = values.size();
  
  // For small inputs, use single-threaded approach
  if (total_values < 100000) {
    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    for (size_t i = 0; i < total_values; ++i) {
      roaring_bitmap_add(bitmap, values[i]);
    }
    roaring_bitmap_run_optimize(bitmap);
    uint64_t card = roaring_bitmap_get_cardinality(bitmap);
    roaring_bitmap_free(bitmap);
    return card;
  }
  
  // For large inputs, use parallel construction
  unsigned int num_threads = std::thread::hardware_concurrency();
  if (num_threads == 0) num_threads = 4;
  if (num_threads > 16) num_threads = 16;
  
  // Partition the values array into chunks for parallel processing
  size_t chunk_size = (total_values + num_threads - 1) / num_threads;
  
  std::vector<roaring_bitmap_t*> bitmaps(num_threads, nullptr);
  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  
  for (unsigned int t = 0; t < num_threads; ++t) {
    size_t start = t * chunk_size;
    size_t end = std::min(start + chunk_size, total_values);
    
    if (start >= total_values) break;
    
    threads.emplace_back([&values, &bitmaps, t, start, end]() {
      roaring_bitmap_t* local_bitmap = roaring_bitmap_create();
      for (size_t i = start; i < end; ++i) {
        roaring_bitmap_add(local_bitmap, values[i]);
      }
      roaring_bitmap_run_optimize(local_bitmap);
      bitmaps[t] = local_bitmap;
    });
  }
  
  for (auto& th : threads) {
    th.join();
  }
  
  // Collect non-null bitmaps
  std::vector<const roaring_bitmap_t*> valid_bitmaps;
  valid_bitmaps.reserve(num_threads);
  for (unsigned int t = 0; t < num_threads; ++t) {
    if (bitmaps[t] != nullptr) {
      valid_bitmaps.push_back(bitmaps[t]);
    }
  }
  
  uint64_t card = 0;
  if (valid_bitmaps.size() == 1) {
    card = roaring_bitmap_get_cardinality(valid_bitmaps[0]);
  } else if (valid_bitmaps.size() > 1) {
    roaring_bitmap_t* result = roaring_bitmap_or_many(valid_bitmaps.size(), valid_bitmaps.data());
    card = roaring_bitmap_get_cardinality(result);
    roaring_bitmap_free(result);
  }
  
  // Free all local bitmaps
  for (unsigned int t = 0; t < num_threads; ++t) {
    if (bitmaps[t] != nullptr) {
      roaring_bitmap_free(bitmaps[t]);
    }
  }
  
  return card;
}