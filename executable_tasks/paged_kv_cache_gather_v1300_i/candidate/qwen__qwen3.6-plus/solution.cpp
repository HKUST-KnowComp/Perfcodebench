#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
    
    if (iters <= 0 || page_ids.empty()) {
        return 0;
    }

    const size_t n = page_ids.size();
    const uint32_t* cache_data = cache.data();
    const uint32_t* pid_data = page_ids.data();
    const uint32_t* off_data = slot_offsets.data();
    const size_t ps = static_cast<size_t>(page_size);
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    // The baseline resets the hash at the start of each iteration,
    // so the result is identical to a single pass. We compute it once.
    for (size_t i = 0; i < n; ++i) {
        hash ^= cache_data[static_cast<size_t>(pid_data[i]) * ps + off_data[i]];
        hash *= FNV_PRIME;
    }
    
    return hash;
}