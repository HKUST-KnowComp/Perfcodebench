#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    // Bitmap with versioning: stores the last iteration index when the key was seen.
    std::vector<uint32_t> last_seen(key_space, 0);
    uint32_t current_version = 1;
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // If version wraps around, reset the entire vector and start from 1.
        if (current_version == 0) {
            std::fill(last_seen.begin(), last_seen.end(), 0);
            current_version = 1;
        }
        // Mark all enabled keys with the current version.
        for (uint32_t key : enabled_keys) {
            last_seen[key] = current_version;
        }
        // Scan probe keys and sum qualifying values.
        sum = 0;
        const uint32_t* p_probe = probe_keys.data();
        const uint32_t* p_val = values.data();
        std::size_t n = probe_keys.size();
        for (std::size_t i = 0; i < n; ++i) {
            if (last_seen[p_probe[i]] == current_version) {
                sum += static_cast<uint64_t>(p_val[i]);
            }
        }
        ++current_version;
    }
    return sum;
}
