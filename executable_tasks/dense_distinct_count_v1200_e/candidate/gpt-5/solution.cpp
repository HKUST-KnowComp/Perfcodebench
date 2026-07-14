#include "interface.h"

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  // Direct-address table with epoch stamping to avoid per-iteration clears.
  std::vector<uint32_t> marks(key_space, 0u);
  uint32_t epoch = 1u;

  uint64_t last_count = 0;
  const uint32_t* kptr_begin = keys.data();
  const uint32_t* kptr_end = kptr_begin + keys.size();
  uint32_t* marks_data = marks.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_count = 0;

    // Count distinct keys for this iteration using the current epoch.
    const uint32_t* p = kptr_begin;
    for (; p != kptr_end; ++p) {
      uint32_t idx = *p; // Assumes 0 <= idx < key_space per task contract.
      if (marks_data[idx] != epoch) {
        marks_data[idx] = epoch;
        ++iter_count;
      }
    }

    last_count = iter_count;

    // Advance epoch for the next iteration; handle overflow by resetting table.
    ++epoch;
    if (epoch == 0u) {
      // Extremely rare path: clear marks and restart epoch at 1.
      for (uint32_t i = 0; i < key_space; ++i) marks_data[i] = 0u;
      epoch = 1u;
    }
  }

  return last_count;
}
