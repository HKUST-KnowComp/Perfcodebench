#include "interface.h"
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || iters <= 0) return 0;
    
    // Generation array avoids O(key_space) clearing overhead per iteration.
    // Memory: O(key_space), Time: O(key_space + iters * keys.size())
    std::vector<uint32_t> last_seen(key_space, 0);
    uint64_t count = 0;
    
    const uint32_t* k_data = keys.data();
    size_t k_size = keys.size();
    uint32_t* seen_data = last_seen.data();
    
    for (int iter = 1; iter <= iters; ++iter) {
        count = 0;
        uint32_t gen = static_cast<uint32_t>(iter);
        for (size_t i = 0; i < k_size; ++i) {
            uint32_t key = k_data[i];
            if (seen_data[key] != gen) {
                seen_data[key] = gen;
                ++count;
            }
        }
    }
    return count;
}
