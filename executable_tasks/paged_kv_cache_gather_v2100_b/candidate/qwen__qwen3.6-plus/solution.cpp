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
    
    const uint32_t* cache_ptr = cache.data();
    const uint32_t* pids_ptr = page_ids.data();
    const uint32_t* offsets_ptr = slot_offsets.data();
    const size_t n = page_ids.size();
    const size_t ps = static_cast<size_t>(page_size);
    
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            uint32_t val = cache_ptr[pids_ptr[i] * ps + offsets_ptr[i]];
            hash ^= val;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}
