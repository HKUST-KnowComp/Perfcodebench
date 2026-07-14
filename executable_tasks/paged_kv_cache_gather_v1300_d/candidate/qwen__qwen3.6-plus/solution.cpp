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
    
    const size_t n = page_ids.size();
    if (n == 0) return 1469598103934665603ULL;
    
    const uint32_t* cache_ptr = cache.data();
    const uint32_t* pid_ptr = page_ids.data();
    const uint32_t* off_ptr = slot_offsets.data();
    
    std::vector<size_t> indices(n);
    const size_t ps = static_cast<size_t>(page_size);
    for (size_t i = 0; i < n; ++i) {
        indices[i] = static_cast<size_t>(pid_ptr[i]) * ps + static_cast<size_t>(off_ptr[i]);
    }
    
    const size_t* idx_ptr = indices.data();
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            hash ^= cache_ptr[idx_ptr[i]];
            hash *= FNV_PRIME;
        }
    }
    return hash;
}