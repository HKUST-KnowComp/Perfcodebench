#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
    const size_t n = page_ids.size();
    const size_t ps = static_cast<size_t>(page_size);
    std::vector<size_t> indices(n);
    for (size_t i = 0; i < n; ++i) {
        indices[i] = static_cast<size_t>(page_ids[i]) * ps + static_cast<size_t>(slot_offsets[i]);
    }
    uint64_t hash = 0;
    const uint32_t* cache_data = cache.data();
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            hash ^= static_cast<uint64_t>(cache_data[indices[i]]);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
