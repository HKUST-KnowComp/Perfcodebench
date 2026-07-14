#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (iters <= 0) return 0;
    
    // Fallback to sort for extremely large key spaces to avoid excessive memory usage
    if (key_space > 64000000) {
        std::vector<uint32_t> sorted_keys = keys;
        std::sort(sorted_keys.begin(), sorted_keys.end());
        auto last = std::unique(sorted_keys.begin(), sorted_keys.end());
        return static_cast<uint64_t>(std::distance(sorted_keys.begin(), last));
    }
    
    // Generation array avoids O(key_space) clearing cost per iteration
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    const uint32_t* keys_data = keys.data();
    size_t n = keys.size();
    uint32_t* seen_data = seen.data();
    
    for (int iter = 1; iter <= iters; ++iter) {
        count = 0;
        uint32_t iter_u = static_cast<uint32_t>(iter);
        for (size_t i = 0; i < n; ++i) {
            uint32_t k = keys_data[i];
            if (seen_data[k] != iter_u) {
                seen_data[k] = iter_u;
                ++count;
            }
        }
    }
    return count;
}
