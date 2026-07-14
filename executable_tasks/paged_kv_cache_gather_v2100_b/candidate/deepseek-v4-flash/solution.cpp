#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
    const std::size_t ps = static_cast<std::size_t>(page_size);
    const uint32_t* cache_data = cache.data();
    const uint32_t* page_ids_data = page_ids.data();
    const uint32_t* slot_offsets_data = slot_offsets.data();
    const std::size_t n = page_ids.size();

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t pid = static_cast<std::size_t>(page_ids_data[i]);
            std::size_t soff = static_cast<std::size_t>(slot_offsets_data[i]);
            uint64_t value = cache_data[pid * ps + soff];
            hash = mix(hash, value);
        }
    }
    return hash;
}
