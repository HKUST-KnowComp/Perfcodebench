#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    (void)iters;
    static std::vector<uint32_t> stamp;
    static uint32_t tag = 0;
    
    if (stamp.size() < key_space) {
        stamp.assign(key_space, 0);
        tag = 1;
    }
    ++tag;
    if (tag == 0) {
        stamp.assign(stamp.size(), 0);
        tag = 1;
    }
    
    for (auto k : enabled_keys) {
        stamp[k] = tag;
    }
    
    uint64_t sum = 0;
    const auto* probe_ptr = probe_keys.data();
    const auto* val_ptr = values.data();
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
        if (stamp[probe_ptr[i]] == tag) {
            sum += static_cast<uint64_t>(val_ptr[i]);
        }
    }
    return sum;
}
