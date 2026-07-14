#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segmented_reduce_checksum(
    const std::vector<uint32_t>& segment_ids,
    const std::vector<uint32_t>& values,
    uint32_t segments,
    int iters) {
  const std::size_t n = segment_ids.size();
  const std::size_t seg_count = static_cast<std::size_t>(segments);

  std::vector<uint32_t> offsets(seg_count + 1);
  std::vector<uint64_t> sums(seg_count);

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (seg_count != 0) {
      uint32_t* const offsets_data = offsets.data();
      uint64_t* const sums_data = sums.data();

      offsets_data[0] = 0;

      std::size_t i = 0;
      uint32_t current_segment = 0;

      while (i < n && current_segment < segments) {
        const uint32_t sid = segment_ids[i];

        while (current_segment < sid) {
          sums_data[current_segment] = 0;
          offsets_data[static_cast<std::size_t>(current_segment) + 1] =
              offsets_data[current_segment];
          ++current_segment;
        }

        uint64_t sum = 0;
        const std::size_t start = i;
        do {
          sum += static_cast<uint64_t>(values[i]);
          ++i;
        } while (i < n && segment_ids[i] == sid);

        sums_data[sid] = sum;
        offsets_data[static_cast<std::size_t>(sid) + 1] =
            offsets_data[sid] + static_cast<uint32_t>(i - start);
        current_segment = sid + 1;
      }

      while (current_segment < segments) {
        sums_data[current_segment] = 0;
        offsets_data[static_cast<std::size_t>(current_segment) + 1] =
            offsets_data[current_segment];
        ++current_segment;
      }
    }

    hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s <= seg_count; ++s) {
      hash = mix(hash, offsets[s]);
    }
    for (std::size_t s = 0; s < seg_count; ++s) {
      hash = mix(hash, sums[s]);
    }
  }

  return hash;
}
