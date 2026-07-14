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
    
    const uint32_t* c = cache.data();
    const uint32_t* p = page_ids.data();
    const uint32_t* s = slot_offsets.data();
    std::size_t n = page_ids.size();
    
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
        std::size_t idx = static_cast<std::size_t>(p[i]) * page_size + s[i];
        uint32_t val = c[idx];
        hash ^= val;
        hash *= 1099511628211ULL;
    }
    return hash;
}