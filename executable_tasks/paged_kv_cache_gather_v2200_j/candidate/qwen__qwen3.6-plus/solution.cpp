#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
    (void)num_pages;
    (void)iters;

    const uint32_t* cache_data = cache.data();
    const uint32_t* ids_data = page_ids.data();
    const uint32_t* offsets_data = slot_offsets.data();
    size_t n = page_ids.size();
    size_t ps = static_cast<size_t>(page_size);

    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        uint32_t val = cache_data[ids_data[i] * ps + offsets_data[i]];
        hash ^= val;
        hash *= 1099511628211ULL;
    }
    return hash;
}
