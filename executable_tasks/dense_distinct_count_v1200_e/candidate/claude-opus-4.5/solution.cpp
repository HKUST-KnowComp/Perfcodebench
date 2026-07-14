#include "interface.h"

#include <cstring>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (key_space == 0 || keys.empty()) {
    return 0;
  }

  const size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  const size_t byte_size = num_words * sizeof(uint64_t);

  // Use stack for small key_space, heap for large
  constexpr size_t STACK_THRESHOLD = 8192; // 64KB on stack
  uint64_t stack_bitmap[STACK_THRESHOLD];
  std::vector<uint64_t> heap_bitmap;
  uint64_t* bitmap;

  if (num_words <= STACK_THRESHOLD) {
    bitmap = stack_bitmap;
  } else {
    heap_bitmap.resize(num_words);
    bitmap = heap_bitmap.data();
  }

  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitmap, 0, byte_size);

    // Set bits for each key
    for (const uint32_t key : keys) {
      const size_t word_idx = key >> 6;       // key / 64
      const uint64_t bit_mask = 1ULL << (key & 63); // key % 64
      bitmap[word_idx] |= bit_mask;
    }

    // Count set bits using popcount
    count = 0;
    for (size_t i = 0; i < num_words; ++i) {
      count += __builtin_popcountll(bitmap[i]);
    }
  }

  return count;
}
